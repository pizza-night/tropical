#pragma once

#include "util/overload.hpp"

#include <variant>

template <typename VariantType, typename... Fs>
decltype(auto) match(VariantType&& variant, Fs&&... fs) {
    return std::visit(
        overload {std::forward<Fs>(fs)...},
        std::forward<VariantType>(variant)
    );
}
