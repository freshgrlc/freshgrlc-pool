#ifndef SOCKET_SOCKET_H
#define SOCKET_SOCKET_H

#include "SocketBase.h"
#include "Packet.h"


class Socket : public SocketBase
{
    public:
        using SocketBase::SocketBase;
        inline Socket(SocketBase &&socket) : SocketBase(std::move(socket)) {}

        void send(const Packet &packet);

    protected:
        void receive(void);

        static int openConnectionTo(const std::string &host, int port);

    private:
        virtual void onReceive(const Packet &packet) = 0;
};

#endif
