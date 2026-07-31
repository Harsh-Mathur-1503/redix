#include "networking/line_framer.hpp"

namespace redix
{
    LineFramer::Status LineFramer::append(std::string_view data, std::string& request)
    {
        request.clear();
        buffer_.append(data);

        while (true)
        {
            const std::size_t newline_pos = buffer_.find('\n');
            if (newline_pos == std::string::npos)
            {
                if (buffer_.size() > MAX_REQUEST_SIZE)
                {
                    if (!(buffer_.size() == MAX_REQUEST_SIZE + 1 && buffer_.back() == '\r'))
                    {
                        return Status::TooLarge;
                    }
                }
                return Status::Incomplete;
            }

            std::size_t payload_size = newline_pos;

            if (payload_size > 0 && buffer_[payload_size - 1] == '\r')
            {
                --payload_size;
            }

            if (payload_size > MAX_REQUEST_SIZE)
            {
                return Status::TooLarge;
            }

            request = buffer_.substr(0, payload_size);
            buffer_.erase(0, newline_pos + 1);
            return Status::Complete;
        }
    }

} // namespace redix