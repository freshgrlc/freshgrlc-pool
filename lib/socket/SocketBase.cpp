#include "SocketBase.h"

#include <unistd.h>

#include <util.h>


SocketBase::SocketBase(int fd) :
    _fd(fd)
{
}

SocketBase::SocketBase(SocketBase &&socket)
{
    _fd = socket._fd;
    socket._fd = -1;
}

SocketBase::~SocketBase()
{
    this->close();
}

void SocketBase::close()
{
    if (this->connected())
    {
        mlog(DEBUG, "Closing socket %d", _fd);

        ::close(_fd);
        _fd = -1;
    }
}
