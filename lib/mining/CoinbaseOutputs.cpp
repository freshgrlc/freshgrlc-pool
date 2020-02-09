#include "CoinbaseOutputs.h"

#define CORRECTION_FACTOR_PRECISION     1000000

void CoinbaseOutputs::rebalance(uint64_t totalCoins)
{
    uint64_t totalShares = 0;

    if (!this->size())
        return;     /* Cannot rebalance witout any outputs */

    for (auto &output : *this)
        totalShares += output->coins;

    if (totalShares == 0)
        return;     /* Cannot rebalance witout any real outputs */

    if (totalCoins == totalShares)
        return;     /* Already balanced */

    uint64_t correctionFactor = totalCoins * CORRECTION_FACTOR_PRECISION / totalShares;
    uint64_t newTotalCoins = 0;

    for (auto &output : *this)
        newTotalCoins += (output->coins = (uint64_t) output->coins * correctionFactor / CORRECTION_FACTOR_PRECISION);

    if (newTotalCoins != totalCoins)
    {
        /* Rounding error */
        return this->rebalance(totalCoins);
    }
}
