#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace redix
{

class KeyValueStore
{
public:
    void set(
        const std::string& key,
        const std::string& value);

    [[nodiscard]]
    std::optional<std::string> get(
        const std::string& key) const;

    bool del(
        const std::string& key);

    [[nodiscard]]
    bool exists(
        const std::string& key) const;

private:
    std::unordered_map<
        std::string,
        std::string
    > data_;
};

} // namespace redix