#ifndef BITCOINP2P_P2PSOCKET_H
#define BITCOINP2P_P2PSOCKET_H

#include <memory>

#include <socket/AsynchronousSocket.h>
#include <util/ByteString.h>


class P2PConnection;
class P2PMessage;

class P2PSocket;
typedef std::unique_ptr<P2PSocket> P2PSocketRef;

class P2PSocket : public AsynchronousSocket
{
    public:
        inline P2PSocket(P2PConnection &parent, const std::string &host, int port) : AsynchronousSocket(openConnectionTo(host, port), true),
            _parent(parent)
        {
        }

        inline static P2PSocketRef connectToFor(const std::string &host, int port, P2PConnection &parent)
        {
            return std::make_unique<P2PSocket>(parent, host, port);
        }

        void send(const P2PMessage &message);

    private:
        P2PConnection &_parent;
        ByteString _buffer;

        void onReceive(const Packet &packet) override;
};

#endif
