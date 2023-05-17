#pragma once

#include "message.hpp"

#include <memory>

namespace tropical {

class SpscQueue {
  private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    SpscQueue();

    SpscQueue(SpscQueue const&) = delete;
    SpscQueue(SpscQueue&&) noexcept;

    SpscQueue& operator=(SpscQueue const&) = delete;
    SpscQueue& operator=(SpscQueue&&) noexcept;

    ~SpscQueue();

    void enqueue(Message);

    void dequeue(Message&);

    [[nodiscard]]
    int fd() noexcept;
};

} // namespace tropical
