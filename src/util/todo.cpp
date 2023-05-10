#include "util/todo.hpp"

#include <cstdio>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/core.h>

[[noreturn]]
void todo(std::source_location const loc) {
    fmt::print(
        stderr,
        "{} {}\n"
        "at {}:{}\n",
        fmt::styled("unimplemented function", fmt::fg(fmt::color::red)),
        loc.function_name(),
        loc.file_name(),
        loc.line()
    );
    std::fflush(stderr);
    std::abort();
}
