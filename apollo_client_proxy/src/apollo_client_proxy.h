#ifndef __APOLLO_CLIENT_PROXY_H__
#define __APOLLO_CLIENT_PROXY_H__

#include "easy_tcpserver.h"
#include <map>
#include <string>

namespace easy_tcp
{
class apollo_client_proxy
{
public:
    apollo_client_proxy();
    void start(RequestCallbackT request_callback_,
               EventCallbackT event_callback_, short port);
    void stop();
private:
    std::map<std::string, std::map<std::string,std::string>> test_data_map;

    CTcpServer tcp_server_;
};
}; // namespace Easy_tcp

#endif