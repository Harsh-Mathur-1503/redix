#include "parser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace redix
{
namespace
{

std::string toUpper(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char ch)
        {
            return static_cast<char>(std::toupper(ch));
        });

    return value;
}

CommandType stringToCommandType(const std::string& command)
{
    if (command == "SET")
    {
        return CommandType::Set;
    }

    if (command == "GET")
    {
        return CommandType::Get;
    }

    if (command == "DEL")
    {
        return CommandType::Del;
    }

    if (command == "EXISTS")
    {
        return CommandType::Exists;
    }

    if (command == "PING")
    {
        return CommandType::Ping;
    }

    return CommandType::Unknown;
}

bool hasValidArity(
    CommandType type,
    const std::vector<std::string>& arguments)
{
    switch (type)
    {
        case CommandType::Set:
            return arguments.size() == 2;

        case CommandType::Get:
        case CommandType::Del:
        case CommandType::Exists:
            return arguments.size() == 1;

        case CommandType::Ping:
            return arguments.empty();

        case CommandType::Unknown:
            return false;
    }

    return false;
}

} // namespace

Command Parser::parse(const std::string& input) const
{
    Command command;

    std::istringstream stream(input);

    std::vector<std::string> tokens;
    std::string token;

    while (stream >> token)
    {
        tokens.push_back(token);
    }

    if (tokens.empty())
    {
        command.status = ParseStatus::EmptyInput;
        return command;
    }

    command.type = stringToCommandType(toUpper(tokens.front()));

    if (command.type == CommandType::Unknown)
    {
        command.status = ParseStatus::UnknownCommand;

        command.arguments.assign(
            tokens.begin() + 1,
            tokens.end());

        return command;
    }

    command.arguments.assign(
        tokens.begin() + 1,
        tokens.end());

    if (!hasValidArity(command.type, command.arguments))
    {
        command.status = ParseStatus::InvalidArity;
        return command;
    }

    command.status = ParseStatus::Success;
    return command;
}

} // namespace redix