// getaddrinfo(), freeaddrinfo(), gaistrerror()
#if defined(_POSIX_C_SOURCE) and _POSIX_C_SOURCE < 200'112L
#   undef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200'112L
#endif

#include "client.hpp"

#include "util/socket.hpp"

#include <cstddef>
#include <netdb.h>
#include <netinet/in.h>
#include <span>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace tropical {

namespace {

constexpr addrinfo numeric_hostname_hints = {
    .ai_flags = AI_NUMERICHOST // do not perform name resolution.
              | AI_ADDRCONFIG  // only accept addresses supported by the system.
              | AI_ALL | AI_V4MAPPED, // accept IPv6 or IPv4-mapped IPv6.
    .ai_family = AF_UNSPEC,           // accept IPv4 or IPv6... again?
    .ai_socktype = SOCK_STREAM,       // TCP
    .ai_protocol = IPPROTO_TCP,       // TCP... again?
    .ai_addrlen = 0,
    .ai_addr = nullptr,
    .ai_canonname = nullptr,
    .ai_next = nullptr,
};

constexpr addrinfo lookup_hostname_hints = {
    .ai_flags = AI_IDN        // accept internationalized domain names.
              | AI_ADDRCONFIG // only accept addresses supported by the system.
              | AI_ALL | AI_V4MAPPED, // accept IPv6 or IPv4-mapped IPv6.
    .ai_family = AF_UNSPEC,           // accept IPv4 or IPv6... again?
    .ai_socktype = SOCK_STREAM,       // TCP
    .ai_protocol = IPPROTO_TCP,       // TCP... again?
    .ai_addrlen = 0,
    .ai_addr = nullptr,
    .ai_canonname = nullptr,
    .ai_next = nullptr,
};

void make_connections(
    std::span<Peer const> const peers,
    std::vector<std::jthread>& connections,
    MpscQueue<Message>& tx_queue,
    SpscQueue<Message>& rx_queue
) {
    connections.reserve(peers.size());

    for (auto const& peer : peers) {
        connections.emplace_back([&peer, &tx_queue, &rx_queue] {
            addrinfo* result;
            int err = getaddrinfo(
                peer.addr.c_str(),
                nullptr,
                &numeric_hostname_hints,
                &result
            );
            // TODO: error handling

            // Try again, this time with lookup.
            freeaddrinfo(result);
            err = getaddrinfo(
                peer.addr.c_str(),
                nullptr,
                &lookup_hostname_hints,
                &result
            );
            // TODO: error handling

            for (addrinfo const* rp = result; rp != nullptr; rp = rp->ai_next) {
                auto connection = Socket::from_fd(
                    socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)
                );
                if (not connection.is_open()) {
                    // TODO: error handling
                    continue;
                }
                if (connect(connection.fd(), rp->ai_addr, rp->ai_addrlen)
                    == -1) {
                    // TODO: error handling
                    continue;
                }
            }

            freeaddrinfo(result);

            // Keep it alive.
            (void) tx_queue;
            (void) rx_queue;

            // At this point, we have tried to connect to all addresses
            // associated with the peer. If we have not succeeded, we should
            // probably give up.
        });
    }
}

} // namespace

void start_client(
    Config const& config,
    MpscQueue<Message>& tx_queue,
    SpscQueue<Message>& rx_queue
) {
    std::vector<std::jthread> connections;
    make_connections(config.peers(), connections, tx_queue, rx_queue);

    auto server_sock = Socket::from_fd(socket(AF_INET6, SOCK_STREAM, 0));

    sockaddr_in6 const bind_addr = {
        .sin6_family = AF_INET6,
        .sin6_port = config.port(),
        .sin6_addr = IN6ADDR_ANY_INIT,
    };
    if (bind(
            server_sock.fd(),
            reinterpret_cast<sockaddr const*>(&bind_addr),
            sizeof(bind_addr)
        )
        != 0) {
        // TODO: error handling
    }

    if (listen(server_sock.fd(), 1'024) != 0) {
        // TODO: error handling
    }

    for (;;) {
        sockaddr_in6 incoming_addr;
        socklen_t incoming_addr_len = sizeof(incoming_addr);
        auto connection = Socket::from_fd(accept(
            server_sock.fd(),
            reinterpret_cast<sockaddr*>(&incoming_addr),
            &incoming_addr_len
        ));
        if (not connection.is_open()) {
            // TODO: error handling
        }
        // TODO: handle connection
    }
}

} // namespace tropical
