#ifndef BITCOINP2P_MESSAGES_VERSION_H
#define BITCOINP2P_MESSAGES_VERSION_H

#include <stdint.h>

#include "../P2PMessage.h"


namespace bitcoinp2p { namespace messages {


class Version : public P2PMessage
{
    public:
        static const Type Type = VERSION;

        Version(uint32_t protocolVersion = 70016);
        Version(P2PMessage &&data);
};

};};

#endif
