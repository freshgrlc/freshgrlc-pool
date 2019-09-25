#include <cstring>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#include <util.h>

#include "Socket.h"
#include "ErrnoError.h"


void Socket::send(const Packet &packet)
{
    ssize_t len = write(_fd, packet.data, packet.length);

    if (len > 0)
    {
        if ((size_t) len < packet.length)
        {
            mlog(WARNING, "Unable to write entire packet, retrying...");
            this->send(Packet(packet.data + len, packet.length - len));
        }
        return;
    }
    else if (!len)
        mlog(DEBUG, "connection [%p]: Closed by peer", this);
    else
        mlog(ERROR, "connection [%p]: Write error %d", this, errno);
}

void Socket::receive()
{
    uint8_t buffer[8192];
    int len;

    for (;;)
    {
        len = read(_fd, buffer, sizeof(buffer));

        if (len > 0)
            this->onReceive(Packet(buffer, len));
        else
        {
            if (!len)
                mlog(DEBUG, "connection [%p]: Closed by peer", this);
            else
                mlog(ERROR, "connection [%p]: Receive error %d", this, errno);

            this->close();
            break;
        }
    }
}

int Socket::openConnectionTo(const std::string &host, int port)
{
    union {
        struct sockaddr_in ipv4_addr;
        struct sockaddr_in6 ipv6_addr;
    } ip_addr;
    size_t ip_addr_len;
    const uint8_t *ip;

    mlog(DEBUG, "Resolving %s...", host.c_str());

    struct hostent *info;

    if (!(info = gethostbyname2(host.c_str(), AF_INET6)))
        info = gethostbyname2(host.c_str(), AF_INET);

    if (!info || !info->h_addr_list || !*info->h_addr_list)
    {
        mlog(ERROR, "No such host: %s (error %d)", host.c_str(), h_errno);
        return -1;
    }

    switch (info->h_addrtype)
    {
        case AF_INET:
            ip_addr.ipv4_addr.sin_family = AF_INET;
            memcpy(&ip_addr.ipv4_addr.sin_addr, *info->h_addr_list, info->h_length);
            ip_addr.ipv4_addr.sin_port = htons(port);
            ip_addr_len = sizeof(ip_addr.ipv4_addr);

            ip = (const uint8_t *) &ip_addr.ipv4_addr.sin_addr;
            mlog(DEBUG, "Connecting to %d.%d.%d.%d...", ip[0], ip[1], ip[2], ip[3]);
            break;

        case AF_INET6:
            ip_addr.ipv6_addr.sin6_family = AF_INET6;
            memcpy(&ip_addr.ipv6_addr.sin6_addr, *info->h_addr_list, info->h_length);
            ip_addr.ipv6_addr.sin6_port = htons(port);
            ip_addr_len = sizeof(ip_addr.ipv6_addr);

            ip = (const uint8_t *) &ip_addr.ipv6_addr.sin6_addr;
            mlog(DEBUG, "Connecting to %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x...",
                        ip[0], ip[1], ip[2], ip[3],
                        ip[4], ip[5], ip[6], ip[7],
                        ip[8], ip[9], ip[10], ip[11],
                        ip[12], ip[13], ip[14], ip[15]);
            break;

        default:
            mlog(ERROR, "Unexpected address family type %d returned by gethostbyname('%s')", info->h_addrtype, host.c_str());
            return -1;
    }

    int fd = socket(info->h_addrtype, SOCK_STREAM, 0);

    if (fd < 0)
    {
        ERRNO_ERROR();
        return -1;
    }

    if (connect(fd, (const struct sockaddr *) &ip_addr, ip_addr_len) < 0)
    {
        mlog(ERROR, "Failed to connect to '%s:%d': error %d", host.c_str(), port, errno);
        ::close(fd);
        return -1;
    }

    mlog(DEBUG, "Connected to '%s:%d'.", host.c_str(), port);

    return fd;
}
