#pragma once

#include <string>

extern "C" {
#include <netinet/in.h>
}

namespace tropical {

struct Peer {
    std::string name;
    in6_addr ip_addr;
};

} // namespace tropical
