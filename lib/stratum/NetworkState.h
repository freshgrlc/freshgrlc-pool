#ifndef STRATUM_NETWORKSTATE_H
#define STRATUM_NETWORKSTATE_H

#include <stdint.h>

#include <mining/BlockHash.h>

class NetworkState
{
    public:
        const uint32_t version;
        const uint32_t bits;
        const uint32_t miningStartTime;
        const uint32_t blockHeight;
        const BlockHash previousBlock;

        const uint64_t coinbaseCoins;

        NetworkState(uint32_t version, uint32_t bits, uint32_t blockHeight, const BlockHash &previousBlock, uint64_t coinbaseCoins);
        NetworkState(uint32_t version, uint32_t bits, uint32_t notBefore, uint32_t blockHeight, const BlockHash &previousBlock, uint64_t coinbaseCoins);
};

#include <memory>

typedef std::shared_ptr<NetworkState> NetworkStateRef;

#endif
