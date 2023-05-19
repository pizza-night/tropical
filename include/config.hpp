#pragma once

#include "peer.hpp"
#include "util/brief_int.hpp"

#include <expected>
#include <filesystem>
#include <netinet/in.h>
#include <span>
#include <system_error>
#include <variant>
#include <vector>

namespace tropical {

class Config {
  private:
    std::vector<Peer> m_peers;
    in_port_t m_port;

    Config(std::vector<Peer>&& peers, in_port_t port) noexcept;

  public:
    struct IOErr {
      private:
        friend class Config;

        IOErr(std::filesystem::path&& path, std::error_code kind) noexcept;

      public:
        std::filesystem::path path;
        std::error_code kind;
    };

    struct ParseErr {
      private:
        friend class Config;

        ParseErr(
            std::filesystem::path&& path,
            std::string&& reason,
            u32 line
        ) noexcept;

      public:
        std::filesystem::path path;
        std::string reason;
        u32 line;
    };

    struct MissingPortErr {
      private:
        friend class Config;

        explicit MissingPortErr(std::filesystem::path&& path) noexcept;

      public:
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

    [[nodiscard]]
    auto peers() const noexcept -> std::span<Peer const>;

    [[nodiscard]]
    auto port() const noexcept -> in_port_t;
};

} // namespace tropical
