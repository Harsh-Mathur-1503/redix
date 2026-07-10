#include "parser.hpp"
#include "command.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace
{
    std:: string toUpper(std::string value)
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
    if(command == "SET")
    {
        return CommandType::Set;
    }

    if(command == "GET")
    {
        return CommandType::Get;
    }

    if(command == "DEL")
    {
        return CommandType::Del;
    }

    if(command == "EXISTS")
    {
        return CommandType::Exists;
    }

    if(command == "PING")
    {
        return CommandType::Ping;
    }

    return CommandType::Unknown;
}

}

Command Parser::parse(const std::string& input) const
{
    Command command;

    std::istringstream stream(input);

    std::vector<std::string> tokens;
    std::string token;

    while(stream >> token)
    {
        tokens.push_back(token);
    }

    if(tokens.empty())
    {
        return command;
    }

    command.type = stringToCommandType(toUpper(tokens.front()));

    for(std::size_t i=1;i<tokens.size();++i)
    {
        command.arguments.push_back(tokens[i]);
    }

    if(!command.arguments.empty())
    {
        command.key = command.arguments[0];
    }

    if(command.arguments.size()>=2)
    {
        command.value = command.arguments[1];
    }

    return command;
}