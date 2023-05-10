#pragma once

#include <optional>
#include <string>

extern "C" {
#include <netinet/in.h>
}

namespace tropical {

struct Peer {
    std::string addr;
    std::optional<in_port_t> port;
    std::optional<std::string> name;
};

} // namespace tropical
