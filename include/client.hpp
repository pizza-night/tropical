#pragma once

#include "config.hpp"
#include "message.hpp"
#include "mpsc_queue.hpp"
#include "spsc_queue.hpp"

namespace tropical {

void start_client(
    Config const&,
    MpscQueue& tx_queue,
    SpscQueue& rx_queue
);

} // namespace tropical
