#pragma once

#include "config.hpp"
#include "util/socket.hpp"

#include <expected>
#include <netinet/in.h>
#include <system_error>

namespace tropical {

auto listen(in_port_t port) -> std::expected<Socket, std::error_code>;

} // namespace tropical
