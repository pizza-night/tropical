#pragma once

#include "message.hpp"

#include <memory>

namespace tropical {

class MpscQueue {
  private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

  public:
    MpscQueue();

    MpscQueue(MpscQueue const&) = delete;
    MpscQueue(MpscQueue&&) noexcept;

    MpscQueue& operator=(MpscQueue const&) = delete;
    MpscQueue& operator=(MpscQueue&&) noexcept;

    ~MpscQueue();

    void enqueue(Message);

    void dequeue(Message&);

    [[nodiscard]]
    int fd() noexcept;
};

} // namespace tropical
