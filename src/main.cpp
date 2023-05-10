#include "config.hpp"
#include "prog_info.hpp"

#include <cstdlib>
#include <cxxopts.hpp>
#include <exception>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>

using namespace tropical;

#define PROG_PRINT_ERR(fmt_str, ...)                                           \
    fmt::print(                                                                \
        stderr,                                                                \
        "{}: {}: " fmt_str,                                                    \
        fmt::styled(                                                           \
            "tropical",                                                        \
            fmt::emphasis::bold | fmt::fg(fmt::color::white)                   \
        ),                                                                     \
        fmt::styled("error", fmt::emphasis::bold | fmt::fg(fmt::color::red))   \
            __VA_OPT__(, ) __VA_ARGS__                                         \
    )

#define PROG_PRINT(fmt_strm, ...)                                              \
    fmt::print(                                                                \
        "{}: " fmt_strm,                                                       \
        fmt::styled(                                                           \
            "tropical",                                                        \
            fmt::emphasis::bold | fmt::fg(fmt::color::white)                   \
        ) __VA_OPT__(, ) __VA_ARGS__                                           \
    )

int main(int argc, char* argv[]) try {
    auto cli_config
        = cxxopts::Options("tropical", "Implementation of the PIZZA protocol.");

    // clang-format off
    cli_config.add_options()
    (
        "help",
        "Print this help message and exit"
    )(
        "version",
        "Print version information and exit"
    )(
        "config",
        "Path to the configuration file",
        cxxopts::value<std::filesystem::path>()
    )(
        "generate-config",
        "Generate a default configuration file"
    );
    // clang-format on

    cxxopts::ParseResult cli_args = cli_config.parse(argc, argv);

    if (cli_args.count("help")) {
        fmt::print("{}\n", cli_config.help());
        return EXIT_SUCCESS;
    }

    if (cli_args.count("version")) {
        fmt::print(
            "tropical {}.{}.{}\n",
            version_major,
            version_minor,
            version_patch
        );
        return EXIT_SUCCESS;
    }

    if (cli_args.count("generate-config")) {
        auto res = Config::generate_default_config();
        if (! res) {
            auto const& [path, err] = res.error();
            PROG_PRINT_ERR(
                "failed to generate default configuration at '{}': {}\n",
                path.native(),
                err.message()
            );
            return EXIT_FAILURE;
        }
        PROG_PRINT("generated default configuration at '{}'\n", res->native());
        return EXIT_SUCCESS;
    }

    bool const using_default_config = cli_args.count("config") == 0;

    std::expected config = using_default_config
                             ? Config::load_default()
                             : Config::load_from_path(
                                 cli_args["config"].as<std::filesystem::path>()
                             );

    if (! config) {
        auto const& [path, err] = config.error();
        PROG_PRINT_ERR(
            "failed to load configuration '{}': {}\n",
            path.native(),
            err.message()
        );
        if (using_default_config) {
            fmt::print(
                stderr,
                "to generate a default configuration, "
                "run the following command with root privileges:\n"
                "\ttropical --generate-config\n"
            );
        }
        return EXIT_FAILURE;
    }
} catch (std::exception const& e) {
    PROG_PRINT_ERR("{}\n", e.what());
    return EXIT_FAILURE;
}
