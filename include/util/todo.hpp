#pragma once

#include <source_location>

[[noreturn]]
void todo(std::source_location loc = std::source_location::current());
