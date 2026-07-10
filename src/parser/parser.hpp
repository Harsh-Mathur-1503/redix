#pragma once

#include "command.hpp"

#include <string>

namespace redix
{

class Parser
{
public:
    [[nodiscard]] Command parse(const std::string& input) const;
};

} // namespace redix