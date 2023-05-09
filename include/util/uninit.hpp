#pragma once

#include <concepts>
#include <memory>
#include <utility>

template <std::move_constructible T>
union Uninit {
    T value;

    constexpr Uninit() noexcept {}

    Uninit(Uninit const&) = delete;
    Uninit& operator=(Uninit const&) = delete;

    Uninit& operator=(Uninit const&&) = delete;
    Uninit& operator=(Uninit&&) = delete;

    constexpr void operator=(T&& value) {
        std::construct_at(std::addressof(this->value), std::move(value));
    }
};
