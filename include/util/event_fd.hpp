#pragma once

class EventFd {
  private:
    int m_fd;

  public:
    EventFd() noexcept;
    ~EventFd();
    void mark_write() noexcept;
    void mark_read() noexcept;
    int fd() noexcept;
};
