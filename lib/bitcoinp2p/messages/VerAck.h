#ifndef BITCOINP2P_MESSAGES_VERACK_H
#define BITCOINP2P_MESSAGES_VERACK_H

#include "../P2PMessage.h"


namespace bitcoinp2p { namespace messages {


class VerAck : public P2PMessage
{
    public:
        static const Type Type = VERACK;

        inline VerAck(void) : P2PMessage(Type) {}
};

};};

#endif
