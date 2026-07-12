#include "core/command_executor.hpp"

namespace redix
{

CommandExecutor::CommandExecutor(KeyValueStore& store)
    : store_(store)
{
}

std::string CommandExecutor::execute(const Command& command)
{
    if (command.status == ParseStatus::EmptyInput)
    {
        return "ERR empty input";
    }

    if (command.status == ParseStatus::UnknownCommand)
    {
        return "ERR unknown command";
    }

    if (command.status == ParseStatus::InvalidArity)
    {
        return "ERR invalid arity";
    }

    switch (command.type)
    {
        case CommandType::Ping:
            return "PONG";

        case CommandType::Set:
            store_.set(command.arguments[0], command.arguments[1]);
            return "OK";

        case CommandType::Get:
        {
            auto value = store_.get(command.arguments[0]);

            if (!value.has_value())
            {
                return "NIL";
            }

            return value.value();
        }

        case CommandType::Del:
            return store_.del(command.arguments[0]) ? "1" : "0";

        case CommandType::Exists:
            return store_.exists(command.arguments[0]) ? "1" : "0";

        case CommandType::Unknown:
            return "ERR unknown command";
    }

    return "ERR internal error";
}

} // namespace redix