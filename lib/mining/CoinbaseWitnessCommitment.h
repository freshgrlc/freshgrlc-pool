#ifndef MINING_COINBASEWITNESSCOMMITMENT_H
#define MINING_COINBASEWITNESSCOMMITMENT_H

#include "CoinbaseCommitment.h"

#include "Hash256.h"

#include "txscript/Script.h"

class CoinbaseWitnessCommitment : public CoinbaseCommitment
{
    public:
        static const uint8_t MAGIC[4];

        CoinbaseWitnessCommitment(const Hash256 &hash);
        CoinbaseWitnessCommitment(const script::Script &redeemScript);
        CoinbaseWitnessCommitment(const ConstByteStringRef &rawData);

        static CoinbaseWitnessCommitment forCoinbaseOnly(void);

        inline static ConstByteStringRef magic(void)   { return ConstByteStringRef(MAGIC); }
};

#endif
