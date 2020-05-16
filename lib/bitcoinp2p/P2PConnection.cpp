#include "P2PConnection.h"

#include <util/logger.h>

#include "messages/Version.h"
#include "messages/VerAck.h"
#include "messages/Ping.h"
#include "messages/GetAddr.h"


using namespace bitcoinp2p;


P2PConnection::P2PConnection(const std::string &host, int port, const ConstByteStringRef &magic) :
    _host(host),
    _port(port),
    _magic(*((uint32_t *) magic.data)),
    _socket(P2PSocket::connectToFor(host, port, *this))
{
    if (!_socket->receiverRunning())
        throw std::runtime_error("Failed to start P2P socket receiver");

    this->sendVersion();
}

void P2PConnection::sendVersion()
{
    this->_socket->send(messages::Version());
}

void P2PConnection::processMessage(const P2PMessageRef &message)
{
    using namespace messages;

    switch (message->type())
    {
        case P2PMessage::VERSION:
            this->sendMessage(VerAck());
            break;

        case P2PMessage::VERACK:
            this->handshakeComplete();
            break;

        case P2PMessage::PING:
            this->sendMessage(message->as<Ping>().response());
            this->sendMessage(Ping());
            break;
    }
}
