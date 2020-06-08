#ifndef MESSAGEBUSSERVER_H
#define MESSAGEBUSSERVER_H

#include <socket/ListeningConnectionManager.h>


class OpaqueMessage;

class MessageBusServer : public ListeningConnectionManager
{
    public:
        MessageBusServer(int port = 9094);

        void broadcastToClients(const OpaqueMessage &message);

        inline int run(void)    { return this->listen(); }

    private:
        IncomingConnectionRef makeConnection(SocketBase &&socket) override;

        int calculateEffectiveBroadcastClients(const std::string &messageClass);
};

#endif
