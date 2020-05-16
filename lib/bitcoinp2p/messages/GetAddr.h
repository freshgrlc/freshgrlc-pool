#ifndef BITCOINP2P_MESSAGES_GETADDR_H
#define BITCOINP2P_MESSAGES_GETADDR_H

#include "../P2PMessage.h"


namespace bitcoinp2p { namespace messages {


class GetAddr : public P2PMessage
{
    public:
        static const Type Type = GETADDR;

        inline GetAddr(void) : P2PMessage(Type) {}
};

};};

#endif
