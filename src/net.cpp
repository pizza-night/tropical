// clang-format off
// This include order is intentional.
#include "spdlog_config.hpp"
// clang-format on

// getaddrinfo(), freeaddrinfo(), gaistrerror()
#if defined(_POSIX_C_SOURCE) and _POSIX_C_SOURCE < 200'112L
#   undef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200'112L
#endif

#include "net.hpp"

#include "util/brief_int.hpp"
#include "util/errno_ec.hpp"
#include "util/strerror_mt.hpp"
#include "util/todo.hpp"

#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <fmt/chrono.h>
#include <netdb.h>
#include <new>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace tropical {

namespace {

void start_connection_with(Peer const& peer) {
    using namespace std::chrono_literals;

    static constexpr addrinfo hint = {
        .ai_flags
        = AI_IDN        // accept internationalized domain names.
        | AI_ADDRCONFIG // only accept addresses supported by the system.
        | AI_ALL | AI_V4MAPPED,     // accept IPv6 or IPv4-mapped IPv6.
        .ai_family = AF_UNSPEC,     // accept IPv4 or IPv6... again?
        .ai_socktype = SOCK_STREAM, // TCP
        .ai_protocol = IPPROTO_TCP, // TCP... again?
        .ai_addrlen = 0,
        .ai_addr = nullptr,
        .ai_canonname = nullptr,
        .ai_next = nullptr,
    };

    static constexpr auto num_attempts = 5_u32;
    static constexpr auto attempt_cooldown = 5s;

    u32 current_attempt = 0;

ATTEMPT_CONNECTION:
    addrinfo* result;
    int err = getaddrinfo(peer.addr.c_str(), nullptr, &hint, &result);
    switch (err) {
    // Lawful good
    case 0:
        break;

    // Lawful evil
    case EAI_ADDRFAMILY:
    case EAI_FAIL:
    case EAI_NODATA:
    case EAI_NONAME:
    case EAI_SERVICE:
    case EAI_SOCKTYPE:
        SPDLOG_ERROR(
            "Failed to resolve address '{}': {}",
            peer.addr,
            gai_strerror(err)
        );
        return;
    case EAI_SYSTEM:
        SPDLOG_ERROR(
            "Failed to resolve address '{}': {}: {}",
            peer.addr,
            gai_strerror(err),
            strerror_mt(errno)
        );
        return;

    // Chaotic good
    case EAI_AGAIN:
        if (current_attempt < num_attempts) {
            ++current_attempt;
            SPDLOG_WARN(
                "Failed to resolve address '{}': {}. Attempt {}/{}, retrying "
                "in {}...",
                peer.addr,
                gai_strerror(err),
                current_attempt,
                num_attempts,
                attempt_cooldown
            );
            std::this_thread::sleep_for(attempt_cooldown);
            goto ATTEMPT_CONNECTION;
        } else {
            SPDLOG_ERROR(
                "Failed to resolve address '{}': {}. Attempt {}/{}, giving up",
                peer.addr,
                gai_strerror(err),
                current_attempt,
                num_attempts
            );
        }
        return;

    // Neutral evil
    case EAI_MEMORY:
        throw std::bad_alloc();

    // Chaotic evil
    default:
        assert(
            err != EAI_BADFLAGS && "getaddrinfo: incorrect flag combination"
        );
        SPDLOG_ERROR(
            "Failed to resolve address '{}' with unknown error {}",
            peer.addr,
            err
        );
        return;
    }

    for (addrinfo const* rp = result; rp != nullptr; rp = rp->ai_next) {
        auto connection = Socket::from_fd(
            socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)
        );
        if (not connection.is_open()) {
            SPDLOG_WARN(
                "Failed to create socket for peer '{}': {}",
                peer.addr,
                strerror_mt(errno)
            );
            continue;
        }
        if (connect(connection.fd(), rp->ai_addr, rp->ai_addrlen) != 0) {
            SPDLOG_WARN(
                "Failed to connect to peer '{}': {}",
                peer.addr,
                strerror_mt(errno)
            );
            continue;
        }

        freeaddrinfo(result);
        SPDLOG_INFO("Connected to peer '{}'", peer.addr);
    }

    SPDLOG_ERROR("Failed to connect to peer '{}'", peer.addr);
    freeaddrinfo(result);
}

} // namespace

auto listen(in_port_t port) -> std::expected<Socket, std::error_code> {
    auto listener = Socket::from_fd(socket(AF_INET6, SOCK_STREAM, 0));
    if (not listener.is_open()) {
        return std::unexpected(errno_ec());
    }

    sockaddr_in6 bind_addr = {
        .sin6_family = AF_INET6,
        .sin6_port = htons(port),
        .sin6_flowinfo = 0,
        .sin6_addr = IN6ADDR_ANY_INIT,
        .sin6_scope_id = 0,
    };
    if (bind(
            listener.fd(),
            reinterpret_cast<sockaddr*>(&bind_addr),
            sizeof(bind_addr)
        )
        != 0) {
        return std::unexpected(errno_ec());
    }

    if (::listen(listener.fd(), 1'024) != 0) {
        return std::unexpected(errno_ec());
    }

    return listener;
}

} // namespace tropical
