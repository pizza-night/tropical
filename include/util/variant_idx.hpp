#pragma once

#include <cstddef>
#include <variant>

template <typename VariantType, typename T, std::size_t idx = 0>
[[nodiscard]]
consteval std::size_t variant_idx() {
    static_assert(
        idx < std::variant_size_v<VariantType>,
        "Type not found in variant"
    );

    if constexpr (idx == std::variant_size_v<VariantType>) {
        return idx;
    } else if constexpr (std::is_same_v<
                             std::variant_alternative_t<idx, VariantType>,
                             T>) {
        return idx;
    } else {
        return variant_idx<VariantType, T, idx + 1>();
    }
}
