#pragma once

#include "parser/command.hpp"
#include "storage/key_value_store.hpp"

#include <string>

namespace redix
{
    class CommandExecutor
    {
        public:
            explicit CommandExecutor(
                KeyValueStore& store
            );
            [[nodiscard]]
            std::string execute(
                const Command& command
            );

        private:
            KeyValueStore& store_;
    };
}