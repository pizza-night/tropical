#pragma once

#include <cstdint>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

constexpr std::uint8_t operator""_u8(unsigned long long n) noexcept {
    return static_cast<std::uint8_t>(n);
}

constexpr std::uint16_t operator""_u16(unsigned long long n) noexcept {
    return static_cast<std::uint16_t>(n);
}

constexpr std::uint32_t operator""_u32(unsigned long long n) noexcept {
    return static_cast<std::uint32_t>(n);
}

constexpr std::uint64_t operator""_u64(unsigned long long n) noexcept {
    return static_cast<std::uint64_t>(n);
}

constexpr std::int8_t operator""_i8(unsigned long long n) noexcept {
    return static_cast<std::int8_t>(n);
}

constexpr std::int16_t operator""_i16(unsigned long long n) noexcept {
    return static_cast<std::int16_t>(n);
}

constexpr std::int32_t operator""_i32(unsigned long long n) noexcept {
    return static_cast<std::int32_t>(n);
}

constexpr std::int64_t operator""_i64(unsigned long long n) noexcept {
    return static_cast<std::int64_t>(n);
}
