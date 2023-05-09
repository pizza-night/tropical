#pragma once

#include "util/event_fd.hpp"
#include "util/uninit.hpp"

#include <cassert>
#include <concepts>
#include <concurrentqueue/concurrentqueue.h>
#include <new>
#include <utility>

namespace tropical {

template <std::move_constructible T>
class MpscQueue
  : protected moodycamel::ConcurrentQueue<T>
  , public EventFd {
  public:
    void enqueue(T&& item) {
        if (! moodycamel::ConcurrentQueue<T>::enqueue(std::move(item))) {
            throw std::bad_alloc();
        }
        EventFd::mark_write();
    }

    T dequeue() {
        Uninit<T> slot;
        assert(moodycamel::ConcurrentQueue<T>::try_dequeue(slot.value));
        EventFd::mark_read();
        return std::move(slot.value);
    }
};

} // namespace tropical
