#include "easy_tcpserver.h"

#include <utility>

namespace easy_tcp
{
CTcpServer::CTcpServer() : m_started(false) {}

CTcpServer::~CTcpServer() { Destroy(); }

void CTcpServer::Create() {}

void CTcpServer::Destroy() { Stop(); }

void CTcpServer::Start(RequestCallbackT request_callback_,
                       EventCallbackT event_callback_,  CTcpServer* sp)
{
    if (m_started)
        return;
    if (m_server != nullptr)
        return;
    data_sp_ = sp;
    m_server_thread = std::thread(&CTcpServer::ServerThread, this, 0,
                                  request_callback_, event_callback_);

    m_started = true;
}

void CTcpServer::Start(RequestCallbackT request_callback_,
                       EventCallbackT event_callback_, short port, CTcpServer* sp)
{
    if (m_started)
        return;
    if (m_server != nullptr)
        return;
    data_sp_ = sp;
    m_server_thread = std::thread(&CTcpServer::ServerThread, this, port,
                                  request_callback_, event_callback_);

    m_started = true;
}

void CTcpServer::Stop()
{
    if (!m_started)
        return;

    if (m_server == nullptr)
        return;
    if (m_io_service != nullptr)
        m_io_service->stop();
    m_server_thread.join();

    m_started = false;
}

bool CTcpServer::IsConnected()
{
    if (!m_started)
        return false;

    if (m_server == nullptr)
        return false;

    return m_server->is_connected();
}

void CTcpServer::ServerThread(std::uint32_t port_,
                              RequestCallbackT request_callback_,
                              EventCallbackT event_callback_)
{
    m_io_service = std::make_shared<asio::io_service>();
    m_server = std::make_shared<CAsioServer>(*m_io_service,
                                             static_cast<unsigned short>(port_),data_sp_);

    m_server->add_request_callback1(std::move(request_callback_));
    m_server->add_event_callback(std::move(event_callback_));

    m_io_service->run();

    m_server = nullptr;
}
}; // namespace easy_tcp
