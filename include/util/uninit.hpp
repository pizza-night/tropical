#pragma once

#include <concepts>
#include <memory>
#include <utility>

template <typename T>
union Uninit {
    T value;

    constexpr Uninit() noexcept {}

    Uninit(Uninit const&) = delete;
    Uninit& operator=(Uninit const&) = delete;

    Uninit& operator=(Uninit const&&) = delete;
    Uninit& operator=(Uninit&&) = delete;

    ~Uninit() {}

    constexpr void operator=(T&& value
    ) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires std::move_constructible<T>
    {
        std::construct_at(std::addressof(this->value), std::move(value));
    }

    constexpr void manually_drop() noexcept(std::is_nothrow_destructible_v<T>)
        requires std::destructible<T>
    {
        std::destroy_at(std::addressof(this->value));
    }
};
