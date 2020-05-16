#ifndef BITCOINP2P_MESSAGES_PING_H
#define BITCOINP2P_MESSAGES_PING_H

#include <stdint.h>

#include "../P2PMessage.h"
#include "Pong.h"


namespace bitcoinp2p { namespace messages {


class Ping : public Pong
{
    public:
        static const P2PMessage::Type Type = PING;

        inline Ping(uint64_t nonce = getNonceFromSystemRandom()) : Pong(nonce, Type) {}
        inline Ping(P2PMessage &&data) : Pong(std::move(data)) {}

        inline Pong response(void) const    { return Pong(this->nonce()); }

        static uint64_t getNonceFromSystemRandom(void);
};

};};

#endif
