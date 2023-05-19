#pragma once

#include "util/brief_int.hpp"

#include <expected>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace tropical {

class Message {
  public:
    using TextMsg = std::string;
    using Payload = std::variant<std::monostate, TextMsg>;

    Payload payload;

    using TypeIdx = std::uint8_t;
    using TextMsgLen = std::uint16_t;

    struct UnexpectedEofErr {
        std::size_t cursor;
        std::string_view missing_field_name;
    };

    struct TextMsgTooLongErr {
        TextMsgLen len;
    };

    struct UnknownTypeErr {
        TypeIdx type_idx;
    };

    using DeserializeErr
        = std::variant<UnexpectedEofErr, UnknownTypeErr, TextMsgTooLongErr>;

    static constexpr TextMsgLen text_msg_max_len
        = std::numeric_limits<TextMsgLen>::max();

    void serialize_to(std::vector<u8>& out) const;

    [[nodiscard]]
    auto deserialize_from(std::span<u8 const> in)
        -> std::expected<void, DeserializeErr>;
};

} // namespace tropical
