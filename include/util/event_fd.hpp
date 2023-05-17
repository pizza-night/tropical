#pragma once

#include <system_error>

class EventFd {
  private:
    int m_fd;

  public:
    EventFd() noexcept;
    EventFd(EventFd const&) = delete;
    EventFd(EventFd&&) noexcept;
    EventFd& operator=(EventFd const&) = delete;
    EventFd& operator=(EventFd&&) noexcept;
    ~EventFd();

    [[nodiscard]]
    std::error_code manually_close() noexcept;

    void mark_write() noexcept;
    void mark_read() noexcept;

    [[nodiscard]]
    int fd() noexcept;
};
