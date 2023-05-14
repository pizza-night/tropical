#pragma once

#include "util/event_fd.hpp"
#include "util/uninit.hpp"

#include <cassert>
#include <concepts>
#include <concurrentqueue/concurrentqueue.h>
#include <new>
#include <utility>

namespace tropical {

template <std::movable T>
class MpscQueue {
  private:
    moodycamel::ConcurrentQueue<T> impl;
    EventFd event_fd;

  public:
    void enqueue(T item) {
        if (! this->impl.enqueue(std::move(item))) {
            throw std::bad_alloc();
        }
        this->event_fd.mark_write();
    }

    T dequeue() {
        Uninit<T> slot;
        assert(this->impl.try_dequeue(slot));
        this->event_fd.mark_read();
        T value = std::move(slot.value);
        slot.manually_drop();
        return value;
    }

    [[nodiscard]]
    int fd() noexcept {
        return this->event_fd.fd();
    }
};

} // namespace tropical
