#ifndef SOCKET_SOCKETBASE_H
#define SOCKET_SOCKETBASE_H

class SocketBase
{
    public:
        SocketBase(int fd = -1);
        SocketBase(SocketBase &&socket);
        virtual ~SocketBase(void);

        void close(void);

        inline bool connected(void)     { return this->_fd >= 0; }

    protected:
        int _fd;
};

#endif

