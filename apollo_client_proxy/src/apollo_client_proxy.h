#ifndef __APOLLO_CLIENT_PROXY_H__
#define __APOLLO_CLIENT_PROXY_H__

#include "easy_tcpserver.h"
#include "apollo_client_mul_yaml.h"
#include <map>
#include <string>

#define PORT 12345

namespace easy_tcp
{

class apollo_proxy_server : public CTcpServer
{
public:
    using apolloClientSptr = std::shared_ptr<apollo_client::apollo_mul_yaml_client>;

    apollo_proxy_server();
    void Init(const std::string &address, const std::string &cluster,
              const std::map<std::string, std::vector<std::string>> &appid_map, unsigned short port = PORT);

    void StartMonitorService(short port = 8060);

private:
    static int defaultRequestCallback(const std::string &request, std::string &response, CTcpServer *sp);

    static void defaultEventCallback(Server_Event event, const std::string &message, CTcpServer *sp);

    std::map<std::string, apolloClientSptr> getDataMap();

    // for monitor
    std::string generate_html();
    //
    void handle_request(web::http::http_request request);

    void monitor_function();

private:
    std::map<std::string, apolloClientSptr> data_map_;
    RequestCallbackT requestCallback_;
    EventCallbackT eventCallback_;
    std::thread monitor_server_;
    short monitor_port_{8060};
};

}; // namespace easy_tcp

#endif