#include "apollo_client_proxy.h"
#include "basemessage.h"

#include <utility>
#include <cpprest/http_listener.h>

namespace easy_tcp
{

apollo_proxy_server::apollo_proxy_server()
{
    requestCallback_ = defaultRequestCallback;
    eventCallback_ = defaultEventCallback;
}

int apollo_proxy_server::defaultRequestCallback(const std::string &request, std::string &response, CTcpServer *sp)
{
    //get map
    auto *p = dynamic_cast<apollo_proxy_server *>(sp);
    auto data = p->getDataMap();

    // parse protobuf
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
    // std::cout << "appid = " << appid.c_str() << std::endl;
    // for (const auto &item : ns_v) {
    //     std::cout << "ns = " << item.c_str() << std::endl;
    // }
    std::map<std::string, std::string> returnMap;
    for (const auto &i : ns_v) {
        if (data[appid]) {
            // std::cout << "ns = " << i << std::endl;
            auto cl = data[appid];
            auto node = cl->getNsNameConfigNode(i);
            if (node) {
                returnMap.insert({i, node.as<std::string>()});
            } else {
                std::cout << "node no exsit" << std::endl;
                SPDLOG_ERROR("{} not exist", appid);
            }
        }
    }
    //return map info  to client

    EASY_TCP::ResponseContent responseContent;
    responseContent.set_timestamp(getTimestampNow());
    responseContent.set_ack(2200);
    responseContent.set_appid(appid);
    auto ma = responseContent.mutable_namespace_config_map();
    for (const auto item : returnMap) {
        ma->insert({item.first, item.second});
    }
    // std::cout << "map size = " << responseContent.namespace_config_map().size() << std::endl;
    std::string sendstr;
    responseContent.SerializeToString(&sendstr);
    EASY_TCP::MsgContent sendmsg;
    sendmsg.set_version(version);
    sendmsg.set_cmd(EASY_TCP::CMD_TYPE::RESPONSE);
    sendmsg.set_content(sendstr.c_str());
    sendstr.clear();
    sendmsg.SerializeToString(&sendstr);
    response = sendstr;
    // std::cout << "response size = " << response.size() << std::endl;
    return 0;
}

void apollo_proxy_server::defaultEventCallback(Server_Event event, const std::string &message, CTcpServer *sp)
{
    if (event == Server_Event::server_event_connected) {
        SPDLOG_INFO("server_event_connected");
    } else if (event == Server_Event::server_event_disconnected) {
        SPDLOG_INFO("server_event_disconnected");
    } else {
        SPDLOG_INFO("server_event_none");
    }
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

void apollo_proxy_server::StartMonitorService(short port)
{
    monitor_port_ = port;
    monitor_server_ = std::thread(&apollo_proxy_server::monitor_function, this);
}

std::string apollo_proxy_server::generate_html()
{
    std::stringstream ss;
    ss << "<html><head><title>Config Status</title></head>";
    ss << "<body><table border='1'>  <caption><strong>config overview</strong></caption>";
    ss << "<tr><th>app_id</th><th>namespace(config_name)</th><th>status(1:ok,0:err)</th></tr>";
    for (const auto &item : data_map_) {
        for (const auto ns : item.second.get()->getAllNamespaces()) {
            ss << "<tr><td>" << item.first << "</td><td>" << ns << "</td><td>" << item.second.get()->getStatus() << "</td></tr>";
        }
        // ss << "<tr><td>" << item.first << "</td><td>" << item.second.get()->getAllNamespaces().front().c_str() << "</td></tr>";
    }
    ss << "</table><br><button onclick='location.reload();'>Refresh</button></body></html>";

    return ss.str();
}

void apollo_proxy_server::handle_request(web::http::http_request request)
{
    if (request.method() == web::http::methods::GET && request.request_uri().path() == "/") {
        // 设置返回内容和响应码
        web::http::http_response response(web::http::status_codes::OK);
        response.set_body(generate_html(), "text/html");
        request.reply(response);

    } else {
        // 返回404响应
        web::http::http_response response(web::http::status_codes::NotFound);
        response.set_body("Not Found");
        request.reply(response);
    }
}

void apollo_proxy_server::monitor_function()
{
    std::string url("http://localhost:");
    url.append(std::to_string(monitor_port_));
    web::http::experimental::listener::http_listener listener(url);
    // 设置处理请求的回调函数
    listener.support([this](web::http::http_request request) {
        this->handle_request(request);
    });
    listener.open().wait();
    while (true) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

} // namespace easy_tcp