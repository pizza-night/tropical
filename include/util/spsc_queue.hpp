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
class SpscQueue {
  private:
    moodycamel::ReaderWriterQueue<T> impl;
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
        assert(this->impl.try_dequeue(slot.value));
        this->event_fd.mark_read();
        return std::move(slot.value);
    }

    int fd() noexcept {
        return this->event_fd.fd();
    }
};

} // namespace tropical
