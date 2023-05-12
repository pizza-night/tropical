#pragma once

#include "config.hpp"
#include "message.hpp"
#include "util/mpsc_queue.hpp"
#include "util/spsc_queue.hpp"

namespace tropical {

void start_client(
    Config const&,
    MpscQueue<Message>& tx_queue,
    SpscQueue<Message>& rx_queue
);

} // namespace tropical
