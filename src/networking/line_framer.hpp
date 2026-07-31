#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace redix
{
    class LineFramer
    {
    public:
        enum class Status
        {
            Complete,
            Incomplete,
            TooLarge
        };

        [[nodiscard]]
        Status append(std::string_view data, std::string& request);

    private:
        static constexpr std::size_t MAX_REQUEST_SIZE = 4096;

        std::string buffer_;
    };

} // namespace redix
