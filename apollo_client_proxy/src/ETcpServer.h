#ifndef __EASY_TCP_SERVER_H__
#define __EASY_TCP_SERVER_H__

#include <iostream>
#include <string>
#include <thread>
#include <asio.hpp>
#include "basemessage.h"


class TcpServer
{
public:
    TcpServer(asio::io_context &io_context, unsigned short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "New connection from: " << socket.remote_endpoint().address().to_string() << std::endl;
                    std::thread{&TcpServer::do_read, this, std::move(socket)}.detach();
                }
                do_accept();
            });
    }

    void do_read(asio::ip::tcp::socket socket)
    {
        asio::streambuf buffer;
        socket.async_read_some(asio::buffer(data_, max_length),
                               std::bind(&TcpServer::handle_read, this,
                                         std::placeholders::_1, std::placeholders::_2));
    }

    void handle_read(const asio::error_code &ec,
                     size_t bytes_transferred)
    {
        if (!ec) {
            request += std::string(data_, bytes_transferred);
            //handler request.
            std::cout << "request = " << request.size() << std::endl;

            //解析
            TcpHeader *msg = reinterpret_cast<TcpHeader*>(data_);
            std::cout << "msg length = " << msg->length << std::endl;
            std::string msgcontent(data_+ sizeof(msg->length), msg->length);
            EASY_TCP::MsgContent content;
            content.ParseFromString(msgcontent);
            dumpContentInfo(content);
        }

    };

    asio::ip::tcp::acceptor acceptor_;
    enum
    {
        max_length = 64 * 1024
    };
    char data_[max_length];
    std::string request;
};

#endif