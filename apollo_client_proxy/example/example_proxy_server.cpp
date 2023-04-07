#include <iostream>
#include "apollo_client_proxy.h"
#include "basemessage.h"

#define PORT 12345

int reqFunc(const std::string &request, std::string &response)
{
    //TODO: deal with tcp stream contact
    EASY_TCP::MsgContent content;
    content.ParseFromString(request);
    auto version = content.version();
    auto cmd = content.cmd();
    auto inner_content_str = content.content();

    EASY_TCP::RequestContent requestContent;
    requestContent.ParseFromString(inner_content_str);
    auto ts = requestContent.timestamp();

    auto appid = requestContent.appid();

    std::vector<std::string> ns_v;
    for (int i = 0; i < requestContent.namespace_vec_size(); ++i) {
        ns_v.push_back(requestContent.namespace_vec(i));
    }
    std::cout << "appid = " << appid.c_str() << std::endl;
    for (const auto &item : ns_v) {
        std::cout << "ns = " << item.c_str() << std::endl;
    }

    //package response.

    EASY_TCP::ResponseContent responseContent;
    responseContent.set_timestamp(getTimestampNow());
    responseContent.set_ack(2200);
    responseContent.set_appid(appid);
    auto ma = responseContent.mutable_namespace_config_map();
    ma->insert({"key1", "value1"});
    ma->insert({"key2", "value2"});
    std::cout << "map size = " << responseContent.namespace_config_map().size() << std::endl;

    std::string sendstr;
    responseContent.SerializeToString(&sendstr);

    EASY_TCP::MsgContent sendmsg;
    sendmsg.set_version(version);
    sendmsg.set_cmd(EASY_TCP::CMD_TYPE::RESPONSE);
    sendmsg.set_content(sendstr.c_str());
    sendstr.clear();
    sendmsg.SerializeToString(&sendstr);

    response = sendstr;

    std::cout << "response size = " << response.size() << std::endl;
    return 0;
}

void eventFunc(easy_tcp::Server_Event event, const std::string &message)
{
    std::cout << "eventFunc" << std::endl;
}

int main()
{
    //appid:{ns1, ns2, ns3};
    std::map<std::string, std::vector<std::string>> m;
    m.insert({"SampleApp", {"test_yaml.yaml", "test_yaml22.yaml", "public_config_1.yaml"}});
    easy_tcp::apollo_proxy_server server;
    // url：cluster
    server.Init("http://kanon2020.top:8080", "default", m);
    server.StartMonitorService();

    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
