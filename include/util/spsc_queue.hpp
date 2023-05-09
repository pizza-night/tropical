#pragma once

#include "util/event_fd.hpp"
#include "util/uninit.hpp"

#include <cassert>
#include <concepts>
#include <new>
#include <readerwriterqueue.h>
#include <utility>

namespace tropical {

template <std::move_constructible T>
class SpscQueue
  : protected moodycamel::ReaderWriterQueue<T>
  , public EventFd {
  public:
    void enqueue(T&& item) {
        if (! moodycamel::ReaderWriterQueue<T>::enqueue(std::move(item))) {
            throw std::bad_alloc();
        }
        EventFd::mark_write();
    }

    T dequeue() {
        Uninit<T> slot;
        assert(moodycamel::ReaderWriterQueue<T>::try_dequeue(slot.value));
        EventFd::mark_read();
        return std::move(slot.value);
    }
};

} // namespace tropical
