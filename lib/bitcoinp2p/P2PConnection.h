#ifndef BITCOINP2P_P2PCONNECTION_H
#define BITCOINP2P_P2PCONNECTION_H

#include <memory>

#include "P2PSocket.h"
#include "P2PMessage.h"

class P2PConnection
{
    public:
        P2PConnection(const std::string &host, int port, const ConstByteStringRef &magic);
        inline virtual ~P2PConnection(void) {}

        virtual void processMessage(const P2PMessageRef &message);

        void sendVersion(void);
        inline void sendMessage(const P2PMessage &message)  { this->_socket->send(message); }

        inline uint32_t networkMagic(void)                  { return _magic; }

        inline const std::string &host(void)                { return _host; }
        inline int port(void)                               { return _port; }

    protected:
        inline virtual void handshakeComplete(void) {}

    private:
        const std::string _host;
        const int _port;
        uint32_t _magic;
        P2PSocketRef _socket;
};

#endif
