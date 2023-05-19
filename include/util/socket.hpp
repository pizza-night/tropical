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

    [[nodiscard]]
    int fd() noexcept;

    [[nodiscard]]
    bool is_open() const noexcept;

    [[nodiscard]]
    std::error_code manually_close() noexcept;

    [[nodiscard]]
    static Socket from_fd(int) noexcept;
};
