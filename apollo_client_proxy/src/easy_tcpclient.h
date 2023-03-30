#ifndef __EASY_TCPCLIENT_H__
#define __EASY_TCPCLIENT_H__

#include "asio_tcp_base.h"

namespace easy_tcp
{
class CTcpClient
{
public:
    typedef std::function<void(const std::string &data_, bool successful_)> AsyncCallbackT;
    typedef std::function<void(Client_Event event, const std::string &message)> EventCallbackT;

    CTcpClient();
    CTcpClient(const std::string &host_name_, unsigned short port_);

    ~CTcpClient();

    void Create(const std::string &host_name_, unsigned short port_);
    void Destroy();

    bool IsConnected();

    std::string GetHostName() { return m_host_name; }

    bool AddEventCallback(EventCallbackT callback_);
    bool RemEventCallback();

    size_t ExecuteRequest(const std::string &request_, int timeout_, std::string &response_);
    void ExecuteRequestAsync(const std::string &request_, int timeout_, AsyncCallbackT callback);

    // size_t ExecuteRequest(const std::vector<char>& request, int timeout_, std::string& response_);


protected:
    std::string m_host_name;
    std::mutex m_socket_write_mutex;
    std::mutex m_socket_read_mutex;
    std::thread m_async_worker;
    std::shared_ptr<asio::io_service> m_io_service;
    std::shared_ptr<asio::io_service::work> m_idle_work;
    std::shared_ptr<asio::ip::tcp::socket> m_socket;
    EventCallbackT m_event_callback;
    bool m_created;
    bool m_connected;
    std::atomic<bool> m_async_request_in_progress;

private:
    bool SendRequest(const std::string &request_);
    size_t ReceiveResponse(std::string &response_, int timeout_);
    void ReceiveResponseAsync(AsyncCallbackT callback_, int timeout_);
    void ReceiveResponseData(const size_t size, AsyncCallbackT callback_);
    void ExecuteCallback(AsyncCallbackT callback_, const std::string &data_, bool success_);
    std::vector<char> pack_write(const std::string &response);
    
};
}; // namespace easy_tcp

#endif