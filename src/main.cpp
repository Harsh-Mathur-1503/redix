#include "core/request_handler.hpp"
#include "networking/tcp_server.hpp"
#include "storage/key_value_store.hpp"

#include <exception>
#include <iostream>

int main()
{
    try
    {
        redix::KeyValueStore store;

        redix::RequestHandler handler(store);

        redix::TcpServer server(handler);

        server.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Fatal error: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}