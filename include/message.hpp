#pragma once

#include <span>
#include <string>
#include <variant>
#include <vector>

namespace tropical {

class Message {
  public:
    using TextMsg = std::string;
    using Payload = std::variant<std::monostate, TextMsg>;

    enum class DeserializeResult {
        ok,
        unexpected_eof,
        txt_msg_too_long,
    };

    Payload payload;

    void serialize_to(std::vector<std::uint8_t>& out) const;

    DeserializeResult deserialize_from(std::span<std::uint8_t const> in);
};

} // namespace tropical
