#ifndef SOCKET_RECONNECTINGCLIENT_H
#define SOCKET_RECONNECTINGCLIENT_H

#include "AsynchronousSocket.h"


class ReconnectingClient : public AsynchronousSocket
{
    public:
        ReconnectingClient(const std::string &host, int port, bool startReceiver = false);
        ReconnectingClient(std::string &&host, int port, bool startReceiver = false);

    private:
        std::string host;
        int port;

        inline virtual void onReconnect(void) {}

        int openConnection(void);

        void cleanup() override;
};

#endif
