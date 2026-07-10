#pragma once

#include <string>
#include <vector>

namespace redix
{

enum class CommandType
{
    Set,
    Get,
    Del,
    Exists,
    Ping,
    Unknown
};

enum class ParseStatus
{
    Success,
    EmptyInput,
    UnknownCommand,
    InvalidArity
};

struct Command
{
    ParseStatus status = ParseStatus::Success;
    CommandType type   = CommandType::Unknown;

    std::vector<std::string> arguments;
};

} // namespace redix