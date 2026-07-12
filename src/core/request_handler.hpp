#pragma once

#include "core/command_executor.hpp"
#include "parser/parser.hpp"

#include <string>

namespace redix
{
    class RequestHandler
    {
        public:
            explicit RequestHandler(
                KeyValueStore& store);

            [[nodiscard]]
            std::string handleLine(
                const std::string& input);

        private:
            Parser parser_;
            CommandExecutor executor_;        
    };
}