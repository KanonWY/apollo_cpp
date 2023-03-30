#ifndef __EASY_TCPSERVER_H__
#define __EASY_TCPSERVER_H__

#include "asio_tcp_base.h"

namespace easy_tcp
{
class CTcpServer
{
public:
    CTcpServer();
    virtual ~CTcpServer();

    void Create();
    void Destroy();

    void Start(RequestCallbackT request_callback_,
               EventCallbackT event_callback_,CTcpServer* sp);

    void Start(RequestCallbackT request_callback_,
               EventCallbackT event_callback_, short port,CTcpServer* sp);

    void Stop();

    bool IsConnected();

    std::shared_ptr<asio::io_service> GetIOService() { return m_io_service; };
    unsigned short GetTcpPort() { return (m_server ? m_server->get_port() : 0); }

protected:
    void ServerThread(std::uint32_t port_, RequestCallbackT request_callback_,
                      EventCallbackT event_callback_);

    bool m_started;

    std::shared_ptr<asio::io_service> m_io_service;
    std::shared_ptr<CAsioServer> m_server;
    std::thread m_server_thread;
    CTcpServer *data_sp_;
};
}; // namespace easy_tcp

#endif