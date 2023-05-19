#pragma once

#include "peer.hpp"
#include "util/brief_int.hpp"

#include <expected>
#include <filesystem>
#include <netinet/in.h>
#include <system_error>
#include <variant>
#include <vector>

namespace tropical {

class Config {
  public:
    std::vector<Peer> peers;
    in_port_t port;

    struct IOErr {
        std::filesystem::path path;
        std::error_code code;
    };

    struct ParseErr {
        std::filesystem::path path;
        std::string reason;
        u32 line;
    };

    struct MissingPortErr {
        std::filesystem::path path;
    };

    using Error = std::variant<IOErr, ParseErr, MissingPortErr>;

    [[nodiscard]]
    static auto generate_default_config()
        -> std::expected<std::filesystem::path, IOErr>;

    [[nodiscard]]
    static auto load_default() -> std::expected<Config, Error>;

    [[nodiscard]]
    static auto load_from_path(std::filesystem::path config_path)
        -> std::expected<Config, Error>;
};

} // namespace tropical
