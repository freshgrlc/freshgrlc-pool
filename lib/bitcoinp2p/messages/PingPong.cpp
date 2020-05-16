#include "Pong.h"
#include "Ping.h"

#include <stdexcept>

#include <util/SystemRandom.h>


using namespace bitcoinp2p::messages;


Pong::Pong(P2PMessage &&data) : P2PMessage(std::move(data))
{
    if (_payload.size() != sizeof(_nonce))
        throw std::runtime_error("P2P Error: Received invalid length for ping/pong packet");

    memcpy(&_nonce, _payload.data(), sizeof(_nonce));
}

uint64_t Ping::getNonceFromSystemRandom()
{
    static SystemRandom random;

    uint64_t nonce;
    random.fill(nonce);
    return nonce;
}
