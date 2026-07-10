#pragma once

#include <string>

#include "command.hpp"

class Parser
{
public:
    Command parse(const std::string& input) const;
};