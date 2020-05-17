#include "NetworkState.h"

#include <ctime>

#include <util/logger.h>

static uint32_t initMiningStartTime(uint32_t notBefore = 0)
{
    uint32_t now = (uint32_t) std::time(NULL);

    if (now < notBefore)
    {
        mlog(WARNING, "Time skew detected: current time %lu, but block cannot be mined before %lu!", now, notBefore);
        now = notBefore;
    }

    return now;
}

NetworkState::NetworkState(uint32_t version, uint32_t bits, uint32_t blockHeight, const BlockHash &previousBlock, const Hash256 &miningTarget, uint64_t coinbaseCoins) :
    version(version),
    bits(bits),
    miningStartTime(initMiningStartTime()),
    blockHeight(blockHeight),
    previousBlock(previousBlock),
    miningTarget(miningTarget),
    miningDiff(Hash256::diff1() / miningTarget / 4096),
    coinbaseCoins(coinbaseCoins)
{
}

NetworkState::NetworkState(uint32_t version, uint32_t bits, uint32_t notBefore, uint32_t blockHeight, const BlockHash &previousBlock, const Hash256 &miningTarget, uint64_t coinbaseCoins) :
    version(version),
    bits(bits),
    miningStartTime(initMiningStartTime(notBefore)),
    blockHeight(blockHeight),
    previousBlock(previousBlock),
    miningTarget(miningTarget),
    miningDiff(Hash256::diff1() / miningTarget / 4096),
    coinbaseCoins(coinbaseCoins)
{
}

bool NetworkState::operator!=(const NetworkState &them) const
{
    /*
     *  Although this is operator abuse, there really is not much
     *  point checking the other stuff.
     */

    if (this->blockHeight != them.blockHeight)
        return true;

    if (!(this->previousBlock == them.previousBlock))
        return true;

    return false;
}
