#ifndef NODECONNECTION_H
#define NODECONNECTION_H


#include <bitcoinp2p/P2PConnection.h>
#include <bitcoinp2p/messages/GetAddr.h>


using namespace bitcoinp2p;

class ConnectionManager;

class NodeConnection : public P2PConnection
{
    public:
        inline NodeConnection(const std::string &host, int port, const ConstByteStringRef &magic, ConnectionManager &parent) : P2PConnection(host, port, magic), parent(parent), _connected(false) {}

        inline bool connected(void)     { return _connected; }

    private:
        ConnectionManager &parent;
        bool _connected;

        inline void handshakeComplete() override
        {
            _connected = true;
            this->sendMessage(messages::GetAddr());
        }

        void processMessage(const P2PMessageRef &message) override;
};

#endif
