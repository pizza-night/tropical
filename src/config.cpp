#include "config.hpp"

#include "util/todo.hpp"

#include <fmt/os.h>
#include <string_view>
#include <utility>

namespace tropical {

namespace {

constexpr std::string_view default_config_contents = 1 + R"(
# This file was automatically generated by tropical. Do not edit, as your
# changes will be overwritten. If you wish to change the configuration, copy
# this file to ~/.config/tropical/config.toml and edit that file instead.
#
# Tropical will listen on port 2504 by default.
# To change this, edit the 'port' value below.
port = 2504

# Tropical will connect to the peers listed in the 'peers' array.
# To add a peer, add a new entry to the array.
# Each entry must be an inline table with the following fields:
#   - 'host': the hostname or IP address of the peer
#   - 'port': the port on which the peer is listening. If omitted, defaults to
#             the value of the 'port' field above.
#
# Example:
# peers = [
#     { host = "192.168.1.1", port = 9999 },
#     { host = "example.com", port = 1337 },
#     { host = "localhost"                },
# ]
peers = []
)";

bool find_config_path(std::filesystem::path& path) {
    char const* const config_dir = std::getenv("XDG_CONFIG_HOME");
    if (! config_dir) {
        char const* const home_dir = std::getenv("HOME");
        if (! home_dir) {
            return false;
        }
        path.assign(home_dir).append(".config");
    } else {
        path = config_dir;
    }
    path.append("tropical").append("config.toml");
    return true;
}

void find_default_config_path(std::filesystem::path& path) {
    path.assign(std::getenv("$XDG_CONFIG_DIRS") ?: "/etc/xdg")
        .append("tropical")
        .append("config.toml");
}

} // namespace

Config::Error::IO::IO(
    std::filesystem::path&& path,
    std::error_code const kind
) noexcept
  : path(std::move(path)), kind(kind) {}

Config::Config(std::vector<Peer>&& peers, in_port_t const port)
  : m_peers(std::move(peers)), m_port(port) {}

auto Config::generate_default_config() -> std::expected<void, Error::IO> {
    std::filesystem::path config_dir;
    find_default_config_path(config_dir);

    std::error_code err;
    if (! std::filesystem::create_directories(config_dir.parent_path(), err)
        and err) {
        return std::unexpected(Error::IO(std::move(config_dir), err));
    }

    try {
        fmt::output_file(config_dir.native())
            .print("{}", default_config_contents);
    } catch (std::system_error const& e) {
        return std::unexpected(Error::IO(std::move(config_dir), e.code()));
    }

    return {};
}

auto Config::load_default() -> std::expected<Config, Config::Error::IO> {
    std::filesystem::path config_path;
    if (! find_config_path(config_path)) {
        find_default_config_path(config_path);
    }
    return Config::load_from_path(std::move(config_path));
}

auto Config::load_from_path(std::filesystem::path)
    -> std::expected<Config, Config::Error::IO> {
    todo();
}

auto Config::peers() const noexcept -> std::span<Peer const> {
    return m_peers;
}

auto Config::port() const noexcept -> in_port_t {
    return m_port;
}

} // namespace tropical
