#include "message.hpp"

#include "util/variant_idx.hpp"

#include <bit>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>

#define lte(n1, n2)                                                            \
    (static_cast<std::uintmax_t>(n1) <= static_cast<std::uintmax_t>(n2))

namespace tropical {

namespace {

template <typename T>
constexpr std::size_t type_idx_of = variant_idx<Message::Payload, T>();

constexpr auto type_idx_max = std::numeric_limits<Message::TypeIdx>::max();

static_assert(
    std::is_same_v<u8, char> or std::is_same_v<u8, unsigned char>,
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
    Message::text_msg_max_len <= Message::TextMsg().max_size(),
    "Text message maximum length too big"
);

template <std::integral N>
constexpr void write_int_be(std::vector<u8>& out, N n) {
    static constexpr auto host_to_be = [](N n) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return n;
        } else {
            return std::byteswap(n);
        }
    };

    if constexpr (std::is_same_v<N, u8>) {
        out.push_back(n);
    } else {
        N n_be = host_to_be(n);
        u8* n_beg = reinterpret_cast<u8*>(&n_be);
        u8* n_end = n_beg + sizeof(n_be);
        out.insert(out.end(), n_beg, n_end);
    }
}

template <std::integral N>
constexpr bool read_int_be(std::span<u8 const> in, N& out) noexcept {
    static constexpr auto be_to_host = [](N n) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return n;
        } else {
            return std::byteswap(n);
        }
    };

    if (in.size() < sizeof(N)) {
        return false;
    }
    out = be_to_host(*reinterpret_cast<N const*>(in.data()));
    return true;
}

struct PayloadSerializer {
    std::vector<u8>& out;

    void operator()(std::monostate) const {
        std::unreachable();
    }

    void operator()(Message::TextMsg const& s) const {
        // Write the string length.
        std::size_t len = s.length();
        assert(lte(len, Message::text_msg_max_len) && "Text message too long");
        write_int_be(out, static_cast<Message::TextMsgLen>(len));

        // Write the string.
        auto s_begin = reinterpret_cast<u8 const*>(s.data());
        auto s_end = s_begin + len;
        out.insert(out.end(), s_begin, s_end);
    }
};

} // namespace

void Message::serialize_to(std::vector<u8>& out) const {
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
std::expected<void, Message::DeserializeErr>
Message::deserialize_from(std::span<u8 const> in) {
    std::size_t cursor = 0;
    TypeIdx type_idx;
    if (! read_int_be(in, type_idx)) {
        return std::unexpected(UnexpectedEofErr {
            .cursor = 0,
            .missing_field_name = "type index",
        });
    }
    cursor += sizeof(TypeIdx);

    switch (static_cast<std::size_t>(type_idx)) {
    case type_idx_of<TextMsg> - 1: {
        TextMsgLen len;
        if (! read_int_be(in.subspan(cursor), len)) {
            return std::unexpected(UnexpectedEofErr {
                .cursor = cursor,
                .missing_field_name = "text message length",
            });
        }
        cursor += sizeof(TextMsgLen);

        if (not lte(len, Message::text_msg_max_len)) {
            return std::unexpected(TextMsgTooLongErr(len));
        }

        if (in.size() < len) {
            return std::unexpected(UnexpectedEofErr {
                .cursor = cursor,
                .missing_field_name = "text message",
            });
        }

        auto s_begin = reinterpret_cast<char const*>(in.subspan(cursor).data());
        auto s_end = s_begin + len;
        if (auto* old_text_msg = std::get_if<TextMsg>(&this->payload);
            old_text_msg != nullptr) {
            // Reuse the existing buffer.
            old_text_msg->assign(s_begin, s_end);
        } else {
            // Create a new one.
            this->payload = TextMsg(s_begin, s_end);
        }
        cursor += len;
    }
        return {};
    default:
        return std::unexpected(UnknownTypeErr(type_idx));
    }
}

} // namespace tropical
