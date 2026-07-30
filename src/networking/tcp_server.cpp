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

        constexpr int         BACKLOG     = 5;
        constexpr std::size_t BUFFER_SIZE = 4096;

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
        std::string       request;
        ReadRequestStatus status = readRequest(client_socket, request);

        switch (status)
        {

        case ReadRequestStatus::Complete:
        {
            std::string response = handler_.handleLine(request);
            response.push_back('\n');

            if (!sendAll(client_socket, response))
            {
                std::cerr << "Failed to send response to client.\n";
            }
            break;
        }

        case ReadRequestStatus::TooLarge:
        {
            sendAll(client_socket, "ERR request too large\n");
            break;
        }

        case ReadRequestStatus::ClientDisconnected:
        {
            // Client close the connection before
            // sending a complete request
            break;
        }

        case ReadRequestStatus::Error:
        {
            // readRequest() already logged the error.
            break;
        }
        }
        close(client_socket);
    }

    TcpServer::ReadRequestStatus TcpServer::readRequest(int client_socket, std::string& request)
    {
        request.clear();
        char buffer[BUFFER_SIZE];

        while (true)
        {
            ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytes_received < 0)
            {
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

            request.append(buffer, static_cast<std::size_t>(bytes_received));

            std::size_t newline_pos = request.find('\n');

            if (newline_pos != std::string::npos)
            {
                // Reject requests whose payload (excluding '\n)
                // exceeds the maximum allowed size
                if (newline_pos > MAX_REQUEST_SIZE)
                {
                    return ReadRequestStatus::TooLarge;
                }

                // v0 processes only the first complete request line.
                // Any bytes after the first '\n' are discarded because
                // the connection is closed after one request.
                request.erase(newline_pos);

                // Remove optional '\r'
                if (!request.empty() && request.back() == '\r')
                {
                    request.pop_back();
                }

                return ReadRequestStatus::Complete;
            }

            // No newline yet.
            // Entire accumulates request counts toward limit.
            if (request.size() > MAX_REQUEST_SIZE)
            {
                return ReadRequestStatus::TooLarge;
            }
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