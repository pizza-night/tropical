#include "util/socket.hpp"

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

int Socket::fd() noexcept {
    return m_fd;
}

bool Socket::open() const noexcept {
    return m_fd != -1;
}

std::error_code Socket::manually_drop() noexcept {
    return std::error_code(
        (m_fd != -1 and close(m_fd) == -1) ? errno : 0,
        std::system_category()
    );
}

Socket Socket::from_fd(int const fd) noexcept {
    return Socket(fd);
}
