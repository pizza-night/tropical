#pragma once

#include "peer.hpp"

#include <expected>
#include <filesystem>
#include <span>
#include <system_error>
#include <vector>

extern "C" {
#include <netinet/in.h>
}

namespace tropical {

class Config {
  private:
    std::vector<Peer> m_peers;
    in_port_t m_port;

    Config(std::vector<Peer>&& peers, in_port_t port);

  public:
    struct Error {
        struct IO {
            std::filesystem::path path;
            std::error_code kind;

            IO(std::filesystem::path&& path, std::error_code kind) noexcept;
        };
    };

    static auto generate_default_config() -> std::expected<void, Error::IO>;

    static auto load_default() -> std::expected<Config, Error::IO>;

    static auto load_from_path(std::filesystem::path config_path)
            -> std::expected<Config, Error::IO>;

    auto peers() const noexcept -> std::span<Peer const>;

    auto port() const noexcept -> in_port_t;
};

} // namespace tropical
