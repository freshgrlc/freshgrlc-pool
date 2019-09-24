#ifndef SOCKET_SOCKET_H
#define SOCKET_SOCKET_H

class Socket
{
    public:
        Socket(int fd = -1);
        Socket(Socket &&socket);
        virtual ~Socket(void);

        void close(void);

        inline bool connected(void)     { return this->_fd >= 0; }

    protected:
        int _fd;
};

#endif
