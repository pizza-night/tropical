#include "util/errno_ec.hpp"

#include <cerrno>

std::error_code errno_ec() noexcept {
    return std::error_code(errno, std::system_category());
}
