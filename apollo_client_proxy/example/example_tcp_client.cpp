#include "easy_tcpclient.h"
#include <iostream>
#include <map>
#include <string>
#include "basemessage.h"

void CALLBACK(const std::string &data_, bool successful_)
{
    std::cout << "callback ok" << successful_ << std::endl;
    std::cout << "data size = " << data_.size() << std::endl;
    EASY_TCP::MsgContent msgContent;
    msgContent.ParseFromString(data_);
    auto version = msgContent.version();
    auto cmd = msgContent.version();
    auto content = msgContent.content();
    std::cout << "version = " << version << std::endl;
    EASY_TCP::ResponseContent responseContent;
    responseContent.ParseFromString(content);

    auto ts = responseContent.timestamp();
    auto appid = responseContent.appid();
    std::map<std::string, std::string> kv;
    std::cout << "size map = " << responseContent.namespace_config_map().size() << std::endl;
    for(const auto& item: responseContent.namespace_config_map())
    {
        kv.insert(item);
    }

    std::cout << "version = " << version << std::endl;
    std::cout << "appid = " << appid << std::endl;
    for(const auto& item :kv)
    {
        std::cout << "key = " << item.first.c_str() << std::endl;
        std::cout << "value = " << item.second.c_str() << std::endl;
    }
}

std::string packSendMsg(const std::string &token, const std::string &appid, const std::vector<std::string> &nsv)
{
    EASY_TCP::RequestContent content_inner;
    content_inner.set_timestamp(getTimestampNow());
    content_inner.set_sequence(1000);
    content_inner.set_host_info("local");
    content_inner.set_token(token);
    content_inner.set_appid(appid);
    for (const auto &item : nsv) {
        content_inner.add_namespace_vec(item.c_str());
    }
    std::string inner_str;
    inner_str.clear();
    content_inner.SerializeToString(&inner_str);

    EASY_TCP::MsgContent content_out;
    content_out.set_version("version_1");
    content_out.set_cmd(EASY_TCP::CMD_TYPE::REQUEST);
    content_out.set_content(inner_str.c_str());
    inner_str.clear();
    content_out.SerializeToString(&inner_str);
    return inner_str;    
}

int main()
{
    easy_tcp::CTcpClient client;
    client.Create("localhost", 12345);

    std::string request("lplpl");

    std::string res;
    res = packSendMsg("","SampleApp",{"test_yaml.yaml"});
    client.ExecuteRequestAsync(res, 200, std::bind(CALLBACK, std::placeholders::_1, std::placeholders::_2));
    std::cout << "res = " << res.size() << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}