#include "util/event_fd.hpp"

#include <sys/eventfd.h>
#include <unistd.h>

EventFd::EventFd() noexcept {
    m_fd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
}

EventFd::~EventFd() {
    close(m_fd);
}

void EventFd::mark_write() noexcept {
    eventfd_write(m_fd, 1);
}

void EventFd::mark_read() noexcept {
    eventfd_t value;
    eventfd_read(m_fd, &value);
}

[[nodiscard]]
int EventFd::fd() noexcept {
    return m_fd;
}
