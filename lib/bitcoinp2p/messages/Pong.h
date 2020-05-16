#ifndef BITCOINP2P_MESSAGES_PONG_H
#define BITCOINP2P_MESSAGES_PONG_H

#include <stdint.h>

#include "../P2PMessage.h"


namespace bitcoinp2p { namespace messages {


class Pong : public P2PMessage
{
    public:
        static const Type Type = PONG;

        inline Pong(uint64_t nonce, P2PMessage::Type type = Type) : P2PMessage(type), _nonce(nonce) { _payload << nonce; }
        Pong(P2PMessage &&data);

        inline uint64_t nonce(void) const   { return _nonce; }

    private:
        uint64_t _nonce;
};

};};

#endif
