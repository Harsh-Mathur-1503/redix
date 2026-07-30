#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(6379);

    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock,
            reinterpret_cast<sockaddr*>(&server),
            sizeof(server));

    send(sock, "PI", 2, 0);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    send(sock, "NG\n", 3, 0);

    char buffer[128];

    ssize_t n = recv(sock,
                     buffer,
                     sizeof(buffer)-1,
                     0);

    if (n > 0)
    {
        buffer[n] = '\0';
        std::cout << buffer;
    }

    close(sock);
}