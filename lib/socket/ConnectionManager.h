#ifndef SOCKET_CONNECTIONMANAGER_H
#define SOCKET_CONNECTIONMANAGER_H

#include <memory>
#include <vector>

#include "Listener.h"
#include "Socket.h"
#include "Packet.h"

#include <Lock.h>
#include <Thread.h>


class IncomingConnection;
typedef std::unique_ptr<IncomingConnection> IncomingConnectionRef;


class ConnectionManager
{
    public:
        class Connections
        {
            public:
                IncomingConnection &add(IncomingConnectionRef &&connection);
                IncomingConnectionRef remove(IncomingConnection &connection, bool take = false);

                template <typename T>
                inline std::vector<std::unique_ptr<T>> &getAs(void)     { return *((std::vector<std::unique_ptr<T>> *) &_clients); }

                inline Lock &lock(void)                                 { return _lock; }

            protected:
                Lock _lock;

            private:
                std::vector<IncomingConnectionRef> _clients;

        };

        typedef std::shared_ptr<Connections> ConnectionsRef;

        ConnectionManager(void);

        void addConnection(SocketBase &&socket);
        static void addConnection(SocketBase &&socket, void *manager);

        inline Connections &connections(void)       { return *connectionsHolder; }

    protected:
        friend class IncomingConnection;

        std::shared_ptr<Connections> connectionsHolder;

        virtual IncomingConnectionRef makeConnection(SocketBase &&socket) = 0;
};


#include <memory>

typedef std::shared_ptr<ConnectionManager> ConnectionManagerRef;

#endif
