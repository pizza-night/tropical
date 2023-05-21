#pragma once

#include "mpsc_queue.hpp"
#include "spsc_queue.hpp"

#include <span>

namespace tropical {

void start_ui(std::span<SpscQueue> tx, MpscQueue& rx);

} // namespace tropical
