#include "networking/tcp_server.hpp"

#include <arpa/inet.h>

#include <cerrno>
#include <cstddef>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

namespace redix
{

    namespace
    {

        constexpr int         BACKLOG          = 5;
        constexpr std::size_t RECV_BUFFER_SIZE = 4096;
        char                  buffer[BUFFER_SIZE];

    } // namespace

    TcpServer::TcpServer(RequestHandler& handler, std::string host, std::uint16_t port)
        : handler_(handler), host_(std::move(host)), port_(port), server_socket_(-1)
    {
    }

    TcpServer::~TcpServer()
    {
        if (server_socket_ >= 0)
        {
            close(server_socket_);
        }
    }

    bool TcpServer::run()
    {
        // socket
        server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

        if (server_socket_ < 0)
        {
            perror("socket");
            return false;
        }

        // setsockopt
        int reuse = 1;
        if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR)");

            close(server_socket_);
            server_socket_ = -1;
            return false;
        }

        sockaddr_in address{};

        address.sin_family = AF_INET;
        address.sin_port   = htons(port_);

        int result = inet_pton(AF_INET, host_.c_str(), &address.sin_addr);

        if (result == 0)
        {
            std::cerr << "Invalid IPv4 address: " << host_ << '\n';

            close(server_socket_);
            server_socket_ = -1;
            return false;
        }

        if (result < 0)
        {
            perror("inet_pton");

            close(server_socket_);
            server_socket_ = -1;
            return false;
        }

        if (bind(server_socket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
        {
            perror("bind");

            close(server_socket_);
            server_socket_ = -1;
            return false;
        }

        if (listen(server_socket_, BACKLOG) < 0)
        {
            perror("listen");

            close(server_socket_);
            server_socket_ = -1;
            return false;
        }

        std::cout << "Redix listening on " << host_ << ':' << port_ << '\n';

        while (true)
        {
            sockaddr_in client_address{};
            socklen_t   client_length;

            int client_socket;

            while (true)
            {
                client_length = sizeof(client_address);
                client_socket = accept(server_socket_, reinterpret_cast<sockaddr*>(&client_address),
                                       &client_length);

                if (client_socket >= 0)
                {
                    break;
                }

                if (errno == EINTR)
                {
                    continue;
                }

                perror("accept");
                client_socket = -1;
                break;
            }

            if (client_socket < 0)
            {
                continue;
            }

#ifdef __APPLE__

            int no_sigpipe = 1;

            if (setsockopt(client_socket, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe,
                           sizeof(no_sigpipe)) < 0)
            {
                perror("setsockopt(SO_NOSIGPIPE)");
                close(client_socket);
                continue;
            }

#endif

            handleClient(client_socket);
        }

        return true;
    }

    void TcpServer::handleClient(int client_socket)
    {
        std::string input_buffer;
        // Process one command at a time until the client
        // disconnects or an error occurs.
        while (true)
        {
            std::string       request;
            ReadRequestStatus status = readRequest(client_socket, input_buffer, request);
            switch (status)
            {
            case ReadRequestStatus::Complete:
            {
                std::string response = handler_.handleLine(request);
                response.push_back('\n');
                if (!sendAll(client_socket, response))
                {
                    close(client_socket);
                    return;
                }
            }
            break;

            case ReadRequestStatus::TooLarge:
            {
                (void)sendAll(client_socket, "ERR request too large\n");
                close(client_socket);
                return;
            }

            case ReadRequestStatus::ClientDisconnected:
            case ReadRequestStatus::Error:
            {
                close(client_socket);
                return;
            }
            }
        }
    }

    TcpServer::ReadRequestStatus
    TcpServer::readRequest(int client_socket, std::string& input_buffer, std::string& request)
    {
        request.clear();

        char buffer[BUFFER_SIZE];

        while (true)
        {
            // First, check whether we already have
            // a complete request buffered.
            std::size_t newline_pos = input_buffer.find('\n');

            if (newline_pos != std::string::npos)
            {
                std::size_t payload_length = newline_pos;

                // Exclude optional '\r' from the payload length.
                if (payload_length > 0 && input_buffer[payload_length - 1] == '\r')
                {
                    --payload_length;
                }

                if (payload_length > MAX_REQUEST_SIZE)
                {
                    return ReadRequestStatus::TooLarge;
                }

                request = input_buffer.substr(0, payload_length);

                // Remove the processed request and its '\n'.
                // Preserve any later bytes for the next request.
                input_buffer.erase(0, newline_pos + 1);

                return ReadRequestStatus::Complete;
            }

            // No complete line yet.
            // Allow:
            //   4096 payload bytes
            //   optional '\r'
            if (input_buffer.size() > MAX_REQUEST_SIZE)
            {
                if (!(input_buffer.size() == MAX_REQUEST_SIZE + 1 && input_buffer.back() == '\r'))
                {
                    return ReadRequestStatus::TooLarge;
                }
            }

            ssize_t bytes_received;

            while (true)
            {
                bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

                if (bytes_received >= 0)
                {
                    break;
                }

                if (errno == EINTR)
                {
                    continue;
                }

                perror("recv");
                return ReadRequestStatus::Error;
            }

            if (bytes_received == 0)
            {
                return ReadRequestStatus::ClientDisconnected;
            }

            input_buffer.append(buffer, static_cast<std::size_t>(bytes_received));
        }
    }
    bool TcpServer::sendAll(int client_socket, const std::string& response)
    {
        std::size_t total_sent = 0;

        while (total_sent < response.size())
        {
            ssize_t bytes_sent =
                send(client_socket, response.data() + total_sent, response.size() - total_sent, 0);

            if (bytes_sent < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                perror("send");
                return false;
            }

            if (bytes_sent == 0)
            {
                std::cerr << "send() wrote 0 bytes.\n";
                return false;
            }

            total_sent += static_cast<std::size_t>(bytes_sent);
        }

        return true;
    }

} // namespace redix