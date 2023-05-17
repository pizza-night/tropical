#include "util/event_fd.hpp"

#include <sys/eventfd.h>
#include <unistd.h>

EventFd::EventFd() noexcept {
    m_fd = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
}

EventFd::EventFd(EventFd&& other) noexcept : m_fd(other.m_fd) {
    other.m_fd = -1;
}

EventFd& EventFd::operator=(EventFd&& other) noexcept {
    if (this != &other) {
        close(m_fd);
        m_fd = other.m_fd;
        other.m_fd = -1;
    }
    return *this;
}

EventFd::~EventFd() {
    if (m_fd != -1) {
        close(m_fd);
    }
}

[[nodiscard]]
std::error_code EventFd::manually_close() noexcept {
    return std::error_code(
        (m_fd != -1 and close(m_fd) == -1) ? errno : 0,
        std::system_category()
    );
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
