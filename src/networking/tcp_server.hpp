#pragma once

#include "core/request_handler.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace redix
{

    class TcpServer
    {
    public:
        TcpServer(RequestHandler& handler, std::string host = "127.0.0.1",
                  std::uint16_t port = 6379);

        ~TcpServer();

        TcpServer(const TcpServer&)            = delete;
        TcpServer& operator=(const TcpServer&) = delete;

        TcpServer(TcpServer&&)            = delete;
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

        // Handles an entire client session until disconnect.
        void handleClient(int client_socket);

        // Reads exactly one newline-delimited request.
        // Any remaining bytes stay in input_buffer.
        [[nodiscard]]
        ReadRequestStatus readRequest(int client_socket, std::string& input_buffer,
                                      std::string& request);

        // Sends the complete response, retrying partial writes.
        [[nodiscard]]
        bool sendAll(int client_socket, const std::string& response);

        RequestHandler& handler_;

        std::string   host_;
        std::uint16_t port_;

        int server_socket_;
    };

} // namespace redix