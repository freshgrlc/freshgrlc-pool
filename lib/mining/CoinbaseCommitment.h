#ifndef MINING_COINBASECOMMITMENT_H
#define MINING_COINBASECOMMITMENT_H

#include "AbstractCoinbaseOutput.h"

#include <util/ByteString.h>


class CoinbaseCommitment : public AbstractCoinbaseOutput
{
    public:
        inline CoinbaseCommitment(const ConstByteStringRef &commitment) : AbstractCoinbaseOutput(0),
            commitment(commitment)
        {}

        const ByteString commitment;

        const script::Script redeemScript(void) const override;

        static bool isCommitment(const AbstractCoinbaseOutput &output);
};

#endif
