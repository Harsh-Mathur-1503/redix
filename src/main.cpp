#include "core/request_handler.hpp"
#include "networking/tcp_server.hpp"
#include "storage/key_value_store.hpp"

#include <cstdlib>

int main()
{
    redix::KeyValueStore  store;
    redix::RequestHandler handler(store);

    redix::TcpServer server(handler);

    if (!server.run())
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}