#pragma once

#include <system_error>

class Socket {
  private:
    int m_fd;

    explicit Socket(int) noexcept;

  public:
    Socket(Socket const&) = delete;
    Socket(Socket&&) noexcept;

    Socket& operator=(Socket const&) = delete;
    Socket& operator=(Socket&&) noexcept;

    ~Socket();

    int fd() noexcept;

    bool open() const noexcept;

    std::error_code manually_drop() noexcept;

    static Socket from_fd(int) noexcept;
};
