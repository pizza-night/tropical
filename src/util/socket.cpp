#include "util/socket.hpp"

#include "util/errno_ec.hpp"

#include <unistd.h>

Socket::Socket(int const fd) noexcept : m_fd(fd) {}

Socket::Socket(Socket&& other) noexcept : m_fd(other.m_fd) {
    other.m_fd = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close(m_fd);
        m_fd = other.m_fd;
        other.m_fd = -1;
    }
    return *this;
}

Socket::~Socket() {
    if (m_fd != -1) {
        close(m_fd);
    }
}

[[nodiscard]]
int Socket::fd() noexcept {
    return m_fd;
}

[[nodiscard]]
bool Socket::is_open() const noexcept {
    return m_fd != -1;
}

[[nodiscard]]
std::error_code Socket::manually_drop() noexcept {
    if (m_fd != -1 and close(m_fd) == -1) {
        return errno_ec();
    }
    return std::error_code();
}

[[nodiscard]]
Socket Socket::from_fd(int const fd) noexcept {
    return Socket(fd);
}
