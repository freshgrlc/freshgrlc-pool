#ifndef MINING_COINBASEOUTPUTS_H
#define MINING_COINBASEOUTPUTS_H

#include "AbstractCoinbaseOutput.h"

#include <vector>


#define SATOSHIS_PER_COIN        100000000UL

/* Pretend every coinbase contains 100 coins, then correct afterwards */
#define TOTAL_COINBASE_COINS    (100 * SATOSHIS_PER_COIN)



class CoinbaseOutputs : public std::vector<AbstractCoinbaseOutputRef>
{
    public:
        inline CoinbaseOutputs(void) : vector() {}
        inline CoinbaseOutputs(CoinbaseOutputs &&src) : vector(std::move(src)) {}

        inline CoinbaseOutputs &operator=(CoinbaseOutputs &&src)
        {
            vector::operator=(std::move(src));
            return *this;
        }

        void rebalance(uint64_t totalCoins = TOTAL_COINBASE_COINS);
};

#endif
