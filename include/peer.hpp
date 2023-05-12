#pragma once

#include <netinet/in.h>
#include <optional>
#include <string>

namespace tropical {

struct Peer {
    std::string addr;
    in_port_t port;
    std::optional<std::string> name;
};

} // namespace tropical
