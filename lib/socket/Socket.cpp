#include <unistd.h>

#include <util.h>

#include "Socket.h"


Socket::Socket(int fd) :
    _fd(fd)
{
}

Socket::Socket(Socket &&socket)
{
    _fd = socket._fd;
    socket._fd = -1;
}

Socket::~Socket()
{
    this->close();
}

void Socket::close()
{
    if (this->connected())
    {
        mlog(DEBUG, "Closing socket %d", _fd);

        ::close(_fd);
        _fd = -1;
    }
}
