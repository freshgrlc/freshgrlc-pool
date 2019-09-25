#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "Listener.h"
#include "ConnectionManager.h"
#include "ErrnoError.h"


Listener::Listener(int port, bool ipv6) : SocketBase(),
    _port(port),
    _ipv6(ipv6)
{
}

Listener::Listener(Listener &&listener) : SocketBase(std::move(listener)),
    _port(listener._port),
    _ipv6(listener._ipv6)
{
}


int Listener::open()
{
    int result;

    if (this->connected())
        return 0;

    if ((result = _ipv6 ? this->open_socket_ipv6() : this->open_socket_ipv4()))
        return result;

    return 0;
}

int Listener::open_socket_ipv4()
{
    struct sockaddr_in ipv4_addr;
    const int enabled = 1;

    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return ERRNO_ERROR();

    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));
    setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &enabled, sizeof(enabled));

    ipv4_addr.sin_family = AF_INET;
    ipv4_addr.sin_addr.s_addr = INADDR_ANY;
    ipv4_addr.sin_port = htons(_port);

    if (bind(_fd, (const struct sockaddr *) &ipv4_addr, sizeof(ipv4_addr)) < 0)
        return ERRNO_ERROR();

    return 0;
}

int Listener::open_socket_ipv6()
{
    struct sockaddr_in6 ipv6_addr;
    const int enabled = 1;
    const int disabled = 0;

    if ((_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        return ERRNO_ERROR();

    if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &disabled, sizeof(disabled)) < 0)
        return ERRNO_ERROR();

    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));
    setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &enabled, sizeof(enabled));

    ipv6_addr.sin6_family = AF_INET6;
    ipv6_addr.sin6_addr = in6addr_any;
    ipv6_addr.sin6_port = htons(_port);

    if (bind(_fd, (const struct sockaddr *) &ipv6_addr, sizeof(ipv6_addr)) < 0)
        return ERRNO_ERROR();

    return 0;
}

int Listener::listen(Listener::IncomingConnectionHandler handler, void *context)
{
    int conn_sock_fd;
    union {
        struct sockaddr_in ipv4_addr;
        struct sockaddr_in6 ipv6_addr;
    } peer;
    socklen_t addr_len;

    if (::listen(_fd, 16) < 0)
        return ERRNO_ERROR();

    for (;;)
    {
        addr_len = _ipv6 ? sizeof(peer.ipv6_addr) : sizeof(peer.ipv4_addr);

        if ((conn_sock_fd = accept(_fd, (struct sockaddr *) &peer, &addr_len)) >= 0)
        {
            SocketBase s(conn_sock_fd);

            try
            {
                handler(std::move(s), context);
            }
            catch (std::exception &e)
            {
                mlog(ERROR, "Failed to spawn handler for new connection %d: %s", conn_sock_fd, e.what());
            }
        }
        else
        {
            mlog(ERROR, "accept() returned %d!", conn_sock_fd);
            return ERRNO_ERROR();
        }
    }
}

int Listener::listen(ConnectionManager& connectionManager)
{
    return this->listen(ConnectionManager::addConnection, &connectionManager);
}


