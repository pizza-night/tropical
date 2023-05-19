#include "mpsc_queue.hpp"

#include "util/event_fd.hpp"

#include <cassert>
#include <concurrentqueue/concurrentqueue.h>
#include <new>
#include <utility>

namespace tropical {

struct MpscQueue::Impl {
    moodycamel::ConcurrentQueue<Message> queue;
    EventFd event_fd;
};

MpscQueue::MpscQueue() {
    this->pimpl = std::make_unique<Impl>();
}

MpscQueue::MpscQueue(MpscQueue&& other) noexcept = default;

MpscQueue& MpscQueue::operator=(MpscQueue&& other) noexcept = default;

void MpscQueue::enqueue(Message msg) {
    if (! this->pimpl->queue.enqueue(std::move(msg))) {
        throw std::bad_alloc();
    }
    this->pimpl->event_fd.mark_write();
}

void MpscQueue::dequeue(Message& slot) {
    assert(this->pimpl->queue.try_dequeue(slot));
    this->pimpl->event_fd.mark_read();
}

[[nodiscard]]
int MpscQueue::fd() noexcept {
    return this->pimpl->event_fd.fd();
}

} // namespace tropical
