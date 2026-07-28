#include "networking/tcp_server.hpp"

#include <arpa/inet.h>

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

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
        perror("setsockopt(SO_REUSEADDR)");

        close(server_socket_);
        server_socket_ = -1;
        return;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);

    int result =
        inet_pton(
            AF_INET,
            host_.c_str(),
            &address.sin_addr);

    if (result == 0)
    {
        std::cerr
            << "Invalid IPv4 address: "
            << host_
            << '\n';

        close(server_socket_);
        server_socket_ = -1;
        return;
    }

    if (result < 0)
    {
        perror("inet_pton");

        close(server_socket_);
        server_socket_ = -1;
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

        close(server_socket_);
        server_socket_ = -1;
        return;
    }

    std::cout
        << "Redix listening on "
        << host_
        << ':'
        << port_
        << '\n';

    while (true)
    {
        sockaddr_in client_address{};
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

#ifdef __APPLE__
        int no_sigpipe = 1;

        if (setsockopt(
                client_socket,
                SOL_SOCKET,
                SO_NOSIGPIPE,
                &no_sigpipe,
                sizeof(no_sigpipe)) < 0)
        {
            perror("setsockopt(SO_NOSIGPIPE)");

            close(client_socket);
            continue;
        }
#endif

        char buffer[BUFFER_SIZE];

        // TODO:
        // recv() does not guarantee that an entire request line
        // arrives in one call. A future version will buffer bytes
        // until a newline is received.
        ssize_t bytes_received =
            recv(
                client_socket,
                buffer,
                BUFFER_SIZE,
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

        std::string request(
            buffer,
            static_cast<std::size_t>(bytes_received));

        while (!request.empty() &&
               (request.back() == '\n' ||
                request.back() == '\r'))
        {
            request.pop_back();
        }

        std::string response =
            handler_.handleLine(request);

        response.push_back('\n');

        // TODO:
        // send() may write fewer bytes than requested.
        // Replace this with a sendAll() helper in Day 7.
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