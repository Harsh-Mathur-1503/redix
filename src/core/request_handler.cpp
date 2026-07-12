#include "core/request_handler.hpp"

namespace redix
{

RequestHandler::RequestHandler(KeyValueStore& store)
    : executor_(store)
{
}

std::string RequestHandler::handleLine(const std::string& input)
{
    Command command = parser_.parse(input);
    return executor_.execute(command);
}

} // namespace redix