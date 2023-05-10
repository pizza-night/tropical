#include "util/todo.hpp"

#include <cstdio>
#include <cstdlib>
#include <fmt/core.h>

[[noreturn]]
void todo(std::source_location const loc) {
    fmt::print(
            stderr,
            "unimplemented '{}' at {}:{}\n",
            loc.function_name(),
            loc.file_name(),
            loc.line()
    );
    std::fflush(stderr);
    std::abort();
}
