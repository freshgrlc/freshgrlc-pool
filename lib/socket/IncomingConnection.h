#ifndef SOCKET_INCOMINGCONNECTION_H
#define SOCKET_INCOMINGCONNECTION_H

#include "Socket.h"
#include "ConnectionManager.h"

#include <memory>

#include <util/Thread.h>


class IncomingConnection : public Socket
{
    public:
        IncomingConnection(ConnectionManager &manager, SocketBase &&socket);

        void moveTo(ConnectionManager &manager);

        inline void enableProcessing(void)  { this->receiver.start(); }
        inline bool dangling(void)          { return _dangling; }

    private:
        class ReceiverThread : public Thread
        {
            public:
                inline ReceiverThread(IncomingConnection *parent) : Thread(), parent(*parent) {}

            private:
                IncomingConnection &parent;

                void main(void) override;
                void initializationCallback(int error) override;
                void cleanupCallback(void) override;
        };

        ReceiverThread receiver;
        ConnectionManager::ConnectionsRef _connections;
        bool _dangling;

        void cleanup(void);
};

typedef std::unique_ptr<IncomingConnection> IncomingConnectionRef;

#endif
