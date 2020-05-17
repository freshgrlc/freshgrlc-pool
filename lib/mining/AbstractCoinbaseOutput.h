#ifndef MINING_ABSTRACTCOINBASEOUTPUT_H
#define MINING_ABSTRACTCOINBASEOUTPUT_H

#include <stdint.h>

#include <util/ByteString.h>

#include "txscript/Script.h"


class AbstractCoinbaseOutput
{
    public:
        uint64_t coins;

        inline virtual ~AbstractCoinbaseOutput(void) {};

        operator const ByteString (void);

        virtual const script::Script redeemScript(void) const = 0;

    protected:
        inline AbstractCoinbaseOutput(uint64_t coins = 0) : coins(coins) {}
};


#include <memory>

typedef std::unique_ptr<AbstractCoinbaseOutput> AbstractCoinbaseOutputRef;

#endif
