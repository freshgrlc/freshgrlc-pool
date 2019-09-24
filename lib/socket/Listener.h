#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include "Socket.h"


class ConnectionManager;

class Listener : public Socket
{
    public:
        typedef void (*IncomingConnectionHandler)(Socket &&socket, void *context);

        Listener(int port, bool ipv6 = true);
        Listener(Listener &&listener);

        int open(void);

        int listen(IncomingConnectionHandler handler, void *context = 0);
        int listen(ConnectionManager &connectionManager);

    private:
        int _port;
        bool _ipv6;

        int open_socket_ipv4(void);
        int open_socket_ipv6(void);
};

#endif
