#ifndef SOCKET_INCOMINGCONNECTION_H
#define SOCKET_INCOMINGCONNECTION_H

#include "AsynchronousSocket.h"
#include "ConnectionManager.h"

#include <memory>

#include <util/Thread.h>


class IncomingConnection : public AsynchronousSocket
{
    public:
        IncomingConnection(ConnectionManager &manager, SocketBase &&socket);

        void moveTo(ConnectionManager &manager);

        inline bool dangling(void)          { return _dangling; }

    private:
        ConnectionManager::ConnectionsRef _connections;
        bool _dangling;

        void cleanup(void) override;
};

typedef std::unique_ptr<IncomingConnection> IncomingConnectionRef;

#endif
