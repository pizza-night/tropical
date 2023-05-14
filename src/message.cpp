#include "message.hpp"

#include "util/variant_idx.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <endian.h>
#include <limits>
#include <type_traits>
#include <utility>

#define lte(n1, n2)                                                            \
    (static_cast<std::uintmax_t>(n1) <= static_cast<std::uintmax_t>(n2))

namespace tropical {

namespace {

using TypeIdx = std::uint8_t;
using TxtMsgSize = std::uint16_t;

template <typename T>
constexpr std::size_t type_idx_of = variant_idx<Message::Payload, T>();

constexpr auto type_idx_max = std::numeric_limits<TypeIdx>::max();

constexpr auto txt_msg_max_len = std::numeric_limits<TxtMsgSize>::max();

static_assert(
    std::is_same_v<std::uint8_t, char>
        or std::is_same_v<std::uint8_t, unsigned char>,
    "std::uint8_t must be a byte type"
);

static_assert(
    type_idx_of<std::monostate> == 0,
    "First message type must be std::monostate"
);

static_assert(
    lte(std::variant_size_v<Message::Payload> - 1,
        static_cast<std::uintmax_t>(type_idx_max) + 1),
    "Too many message types"
);

static_assert(
    txt_msg_max_len <= Message::TextMsg().max_size(),
    "Text message maximum length too large"
);

[[maybe_unused]]
void write_int_be(std::vector<std::uint8_t>& out, std::uint8_t const n) {
    out.push_back(n);
}

[[maybe_unused]]
void write_int_be(std::vector<std::uint8_t>& out, std::uint16_t const n) {
    std::uint16_t const n_be = htobe16(n);
    auto const n_begin = reinterpret_cast<std::uint8_t const*>(&n_be);
    auto const n_end = n_begin + sizeof(n_be);
    out.insert(out.end(), n_begin, n_end);
}

[[maybe_unused]]
void write_int_be(std::vector<std::uint8_t>& out, std::uint32_t const n) {
    std::uint32_t const n_be = htobe32(n);
    auto const n_begin = reinterpret_cast<std::uint8_t const*>(&n_be);
    auto const n_end = n_begin + sizeof(n_be);
    out.insert(out.end(), n_begin, n_end);
}

[[maybe_unused]]
void write_int_be(std::vector<std::uint8_t>& out, std::uint64_t const n) {
    std::uint64_t const n_be = htobe64(n);
    auto const n_begin = reinterpret_cast<std::uint8_t const*>(&n_be);
    auto const n_end = n_begin + sizeof(n_be);
    out.insert(out.end(), n_begin, n_end);
}

[[maybe_unused]]
constexpr bool
read_int_be(std::span<std::uint8_t const>& in, std::uint8_t& out) noexcept {
    if (in.size() < sizeof(out)) {
        return false;
    }
    out = in[0];
    in = in.subspan(sizeof(out));
    return true;
}

[[maybe_unused]]
constexpr bool
read_int_be(std::span<std::uint8_t const>& in, std::uint16_t& out) noexcept {
    if (in.size() < sizeof(out)) {
        return false;
    }
    out = be16toh(*reinterpret_cast<std::uint16_t const*>(in.data()));
    in = in.subspan(sizeof(out));
    return true;
}

[[maybe_unused]]
constexpr bool
read_int_be(std::span<std::uint8_t const>& in, std::uint32_t& out) noexcept {
    if (in.size() < sizeof(out)) {
        return false;
    }
    out = be32toh(*reinterpret_cast<std::uint32_t const*>(in.data()));
    in = in.subspan(sizeof(out));
    return true;
}

[[maybe_unused]]
constexpr bool
read_int_be(std::span<std::uint8_t const>& in, std::uint64_t& out) noexcept {
    if (in.size() < sizeof(out)) {
        return false;
    }
    out = be64toh(*reinterpret_cast<std::uint64_t const*>(in.data()));
    in = in.subspan(sizeof(out));
    return true;
}

struct PayloadSerializer {
    std::vector<std::uint8_t>& out;

    void operator()(std::monostate) const {
        std::unreachable();
    }

    void operator()(Message::TextMsg const& s) const {
        // Write the string length.
        std::size_t const len = s.length();
        assert(lte(len, txt_msg_max_len) && "Text message too long");
        write_int_be(out, static_cast<TxtMsgSize>(len));

        // Write the string.
        auto const s_begin = reinterpret_cast<std::uint8_t const*>(s.data());
        auto const s_end = s_begin + len;
        out.insert(out.end(), s_begin, s_end);
    }
};

} // namespace

void Message::serialize_to(std::vector<std::uint8_t>& out) const {
    // Write the message type.
    std::size_t type_idx = this->payload.index();
    assert(
        (type_idx
         != type_idx_of<
             std::monostate>) &&"Attempted to serialize an empty message"
    );
    --type_idx; // Skip std::monostate.
    assert(lte(type_idx, type_idx_max) && "Message type index out of range");
    write_int_be(out, static_cast<TypeIdx>(type_idx));

    // Write the message payload.
    std::visit(PayloadSerializer(out), this->payload);
}

[[nodiscard]]
Message::DeserializeResult
Message::deserialize_from(std::span<std::uint8_t const> in) {
    std::size_t const type_idx = ({
        TypeIdx type_idx;
        if (! read_int_be(in, type_idx)) {
            return DeserializeResult::unexpected_eof;
        }
        static_cast<std::size_t>(type_idx) + 1;
    });
    switch (type_idx) {
    case type_idx_of<std::monostate>:
        assert(false && "Attempted to deserialize an empty message");
        break;

    case type_idx_of<TextMsg>: {
        TxtMsgSize len;
        if (! read_int_be(in, len)) {
            return DeserializeResult::unexpected_eof;
        }

        if (not lte(len, txt_msg_max_len)) {
            return DeserializeResult::txt_msg_too_long;
        }

        if (in.size() < len) {
            return DeserializeResult::unexpected_eof;
        }

        auto const s_begin = reinterpret_cast<char const*>(in.data());
        auto const s_end = s_begin + len;
        if (auto* const old_txt_msg = std::get_if<TextMsg>(&this->payload);
            old_txt_msg != nullptr) {
            // Reuse the existing buffer.
            old_txt_msg->assign(s_begin, s_end);
        } else {
            // Create a new one.
            this->payload = TextMsg(s_begin, s_end);
        }
        in = in.subspan(len);
        return DeserializeResult::ok;
    }
    default:
        // TODO: Handle unknown message types, for now just ignore them.
        return DeserializeResult::ok;
    }
}

} // namespace tropical
