#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define close_socket closesocket
#else
#include <unistd.h>
using socket_t = int;
#define INVALID_SOCKET_VALUE -1
#define close_socket close
#endif

class SocketWrapper
{
private:
    socket_t m_fd;

public:
    SocketWrapper() : m_fd(INVALID_SOCKET_VALUE) {}

    explicit SocketWrapper(socket_t fd) : m_fd(fd) {}

    ~SocketWrapper()
    {
        if (m_fd != INVALID_SOCKET_VALUE)
        {
            close_socket(m_fd);
        }
    }

    SocketWrapper(SocketWrapper &&other) noexcept : m_fd(other.m_fd)
    {
        other.m_fd = INVALID_SOCKET_VALUE;
    }

    SocketWrapper &operator=(SocketWrapper &&other) noexcept
    {
        if (this != &other)
        {
            if (m_fd != INVALID_SOCKET_VALUE)
            {
                close_socket(m_fd);
            }
            m_fd = other.m_fd;
            other.m_fd = INVALID_SOCKET_VALUE;
        }
        return *this;
    }

    SocketWrapper(const SocketWrapper &) = delete;
    SocketWrapper &operator=(const SocketWrapper &) = delete;

    socket_t get() const { return m_fd; }

    bool is_valid() const { return m_fd != INVALID_SOCKET_VALUE; }

    socket_t release()
    {
        socket_t fd = m_fd;
        m_fd = INVALID_SOCKET_VALUE;
        return fd;
    }

    void reset(socket_t fd = INVALID_SOCKET_VALUE)
    {
        if (m_fd != INVALID_SOCKET_VALUE)
        {
            close_socket(m_fd);
        }
        m_fd = fd;
    }

    operator socket_t() const { return m_fd; }

    explicit operator bool() const { return is_valid(); }
};

#endif // SOCKET_WRAPPER_HPP
