#include "spsc_queue.hpp"

#include "util/event_fd.hpp"

#include <cassert>
#include <new>
#include <readerwriterqueue.h>
#include <utility>

namespace tropical {

struct SpscQueue::Impl {
    moodycamel::ReaderWriterQueue<Message> queue;
    EventFd event_fd;
};

SpscQueue::SpscQueue() {
    this->pimpl = std::make_unique<Impl>();
}

SpscQueue::SpscQueue(SpscQueue&& other) noexcept = default;

SpscQueue& SpscQueue::operator=(SpscQueue&& other) noexcept = default;

void SpscQueue::enqueue(Message msg) {
    if (! this->pimpl->queue.enqueue(std::move(msg))) {
        throw std::bad_alloc();
    }
    this->pimpl->event_fd.mark_write();
}

void SpscQueue::dequeue(Message& slot) {
    assert(this->pimpl->queue.try_dequeue(slot));
    this->pimpl->event_fd.mark_read();
}

[[nodiscard]]
int SpscQueue::fd() noexcept {
    return this->pimpl->event_fd.fd();
}

} // namespace tropical
