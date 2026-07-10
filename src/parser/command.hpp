#pragma once

#include <string>
#include <vector>

enum class CommandType
{
    Set,
    Get,
    Del,
    Exists,
    Ping,
    Unknown
};

struct Command
{
    CommandType type = CommandType::Unknown;

    std::string key;
    std::string value;

    std::vector<std::string> arguments;
};