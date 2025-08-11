#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#include <unistd.h>
#include <utility>

class SocketWrapper
{
private:
    int m_fd;

public:
    SocketWrapper() : m_fd(-1) {}

    explicit SocketWrapper(int fd) : m_fd(fd) {}

    ~SocketWrapper()
    {
        if (m_fd >= 0)
        {
            close(m_fd);
        }
    }

    SocketWrapper(SocketWrapper &&other) noexcept : m_fd(other.m_fd)
    {
        other.m_fd = -1;
    }

    SocketWrapper &operator=(SocketWrapper &&other) noexcept
    {
        if (this != &other)
        {
            if (m_fd >= 0)
            {
                close(m_fd);
            }
            m_fd = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }

    SocketWrapper(const SocketWrapper &) = delete;
    SocketWrapper &operator=(const SocketWrapper &) = delete;

    int get() const { return m_fd; }

    bool is_valid() const { return m_fd >= 0; }

    int release()
    {
        int fd = m_fd;
        m_fd = -1;
        return fd;
    }

    void reset(int fd = -1)
    {
        if (m_fd >= 0)
        {
            close(m_fd);
        }
        m_fd = fd;
    }

    operator int() const { return m_fd; }

    explicit operator bool() const { return is_valid(); }
};

#endif // SOCKET_WRAPPER_HPP
