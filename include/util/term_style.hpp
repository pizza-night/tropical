#pragma once

#include <fmt/color.h>

#define BOLD_WHITE(str)                                                        \
    fmt::styled(str, fmt::emphasis::bold | fmt::fg(fmt::color::white))

#define BOLD_RED(str)                                                          \
    fmt::styled(str, fmt::emphasis::bold | fmt::fg(fmt::color::red))
