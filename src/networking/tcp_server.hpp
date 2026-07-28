#pragma once

#include "core/request_handler.hpp"

#include <cstdint>
#include <string>

namespace redix
{

class TcpServer
{
public:
    TcpServer(
        RequestHandler& handler,
        std::string host = "127.0.0.1",
        std::uint16_t port = 6379);

    ~TcpServer();

    void run();

private:
    RequestHandler& handler_;

    std::string host_;
    std::uint16_t port_;

    int server_socket_;
};

} // namespace redix