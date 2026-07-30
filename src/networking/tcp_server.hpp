#pragma once

#include "core/request_handler.hpp"

#include <cstdint>
#include <string>

namespace redix
{

    // namespace
    // {
    // constexpr int BACKLOG = 5;
    // constexpr std::size_t BUFFER_SIZE = 4096;
    // }

class TcpServer
{
public:
    TcpServer(
        RequestHandler& handler,
        std::string host = "127.0.0.1",
        std::uint16_t port = 6379);

    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(TcpServer&&) = delete;

    [[nodiscard]]
    bool run();

private:
    static constexpr std::size_t MAX_REQUEST_SIZE = 4096;

    enum class ReadRequestStatus
    {
        Complete,
        ClientDisconnected,
        TooLarge,
        Error
    };

    void handleClient(int client_socket);

    [[nodiscard]]
    ReadRequestStatus readRequest(
        int client_socket,
        std::string& request);

    [[nodiscard]]
    bool sendAll(
        int client_socket,
        const std::string& response);

    RequestHandler& handler_;

    std::string host_;
    std::uint16_t port_;

    int server_socket_;
};

} // namespace redix