#include "networking/tcp_server.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <utility>
#include <sys/socket.h>
#include <unistd.h>

namespace redix
{

namespace
{
constexpr int BACKLOG = 5;
constexpr std::size_t BUFFER_SIZE = 4096;
}

TcpServer::TcpServer(
    RequestHandler& handler,
    std::string host,
    std::uint16_t port)
    : handler_(handler),
      host_(std::move(host)),
      port_(port),
      server_socket_(-1)
{
}

TcpServer::~TcpServer()
{
    if (server_socket_ >= 0)
    {
        close(server_socket_);
    }
}

void TcpServer::run()
{
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket_ < 0)
    {
        perror("socket");
        return;
    }

    int reuse = 1;

    if (setsockopt(
            server_socket_,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse,
            sizeof(reuse)) < 0)
    {
        perror("setsockopt");
        return;
    }

    sockaddr_in address {};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);

    if (inet_pton(
            AF_INET,
            host_.c_str(),
            &address.sin_addr) <= 0)
    {
        perror("inet_pton");
        return;
    }

    if (bind(
            server_socket_,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0)
    {
        perror("bind");
        close(server_socket_);
        server_socket_ = -1;
        return;
    }

    if (listen(server_socket_, BACKLOG) < 0)
    {
        perror("listen");
        return;
    }

    std::cout
        << "Redix listening on "
        << host_
        << ":"
        << port_
        << '\n';

    while (true)
    {
        sockaddr_in client_address {};
        socklen_t client_length = sizeof(client_address);

        int client_socket =
            accept(
                server_socket_,
                reinterpret_cast<sockaddr*>(&client_address),
                &client_length);

        if (client_socket < 0)
        {
            perror("accept");
            continue;
        }

        char buffer[BUFFER_SIZE];
// TODO:
// recv() does not guarantee that an entire line arrives in one call.
// For v0 we assume requests fit in one receive.
        ssize_t bytes_received =
            recv(
                client_socket,
                buffer,
                BUFFER_SIZE - 1,
                0);

        if (bytes_received < 0)
        {
            perror("recv");
            close(client_socket);
            continue;
        }

        if (bytes_received == 0)
        {
            close(client_socket);
            continue;
        }

        buffer[bytes_received] = '\0';

        std::string request(buffer);

        while (!request.empty() &&
               (request.back() == '\n' ||
                request.back() == '\r'))
        {
            request.pop_back();
        }
// TODO:
// Handle partial sends by looping until all bytes are written.
        std::string response =
            handler_.handleLine(request);

        response.push_back('\n');

        ssize_t bytes_sent =
            send(
                client_socket,
                response.c_str(),
                response.size(),
                0);

        if (bytes_sent < 0)
        {
            perror("send");
        }

        close(client_socket);
    }
}

} // namespace redix