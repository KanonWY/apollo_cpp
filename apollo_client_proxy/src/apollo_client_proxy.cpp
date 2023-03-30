#include "apollo_client_proxy.h"
#include "basemessage.h"

#include <utility>

namespace easy_tcp
{

apollo_proxy_server::apollo_proxy_server()
{
    requestCallback_ = defaultRequestCallback;
    eventCallback_ = defaultEventCallback;
}

int apollo_proxy_server::defaultRequestCallback(const std::string &request, std::string &response, CTcpServer *sp)
{
    std::cout << "request" << std::endl;
    //get map
    auto *p = dynamic_cast<apollo_proxy_server *>(sp);
    auto data = p->getDataMap();

    // parse protobuf
    EASY_TCP::MsgContent content;
    content.ParseFromString(request);
    std::cout << "content version = " << content.version() << std::endl;
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
    std::map<std::string, std::string> returnMap;
    for (const auto &i : ns_v) {
        if (data[appid]) {
            std::cout << "ns = " << i << std::endl;
            auto cl = data[appid];
            auto node = cl->getNsNameConfigNode(i);
            if (node) {
                returnMap.insert({i, node.as<std::string>()});
            } else {
                std::cout << "node no exsit" << std::endl;
            }
        }
    }
    //return map info  to client

    EASY_TCP::ResponseContent responseContent;
    responseContent.set_timestamp(getTimestampNow());
    responseContent.set_ack(2200);
    responseContent.set_appid(appid);
    auto ma = responseContent.mutable_namespace_config_map();
    for(const auto item : returnMap)
    {
        ma->insert({item.first, item.second});
    }
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

void apollo_proxy_server::defaultEventCallback(Server_Event event, const std::string &message, CTcpServer *sp)
{
    std::cout << "event" << std::endl;
}

void apollo_proxy_server::Init(const std::string &address, const std::string &cluster,
                               const std::map<std::string, std::vector<std::string>> &appid_map, unsigned short port)
{
    for (const auto &item : appid_map) {
        auto apolloClient = std::make_shared<apollo_client::apollo_mul_yaml_client>();
        apolloClient->init(address, item.first, cluster, item.second);
        data_map_.insert({item.first, apolloClient});
    }
    Create();
    Start(requestCallback_, eventCallback_, port, this);
}

std::map<std::string, apollo_proxy_server::apolloClientSptr> apollo_proxy_server::getDataMap()
{
    return data_map_;
}

} // namespace easy_tcp