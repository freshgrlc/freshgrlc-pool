#ifndef MINING_COINBASEOUTPUT_H
#define MINING_COINBASEOUTPUT_H

#include "AbstractCoinbaseOutput.h"

#include "Hash160.h"
#include "txscript/Script.h"


class CoinbaseOutput : public AbstractCoinbaseOutput
{
    public:
        enum Type
        {
            P2PKH,
            P2SH,
            P2WPKH
        };

        const Hash160 destination;
        const Type outputType;

        const script::Script redeemScript() override;

        inline static CoinbaseOutput p2pkh(const Hash160 &destination, uint64_t coins)  { return CoinbaseOutput(P2PKH, destination, coins); }
        inline static CoinbaseOutput p2sh(const Hash160 &destination, uint64_t coins)   { return CoinbaseOutput(P2SH, destination, coins); }
        inline static CoinbaseOutput p2wpkh(const Hash160 &destination, uint64_t coins) { return CoinbaseOutput(P2WPKH, destination, coins); }

    private:
        inline CoinbaseOutput(Type outputType, const Hash160 &destination, uint64_t coins) : AbstractCoinbaseOutput(coins),
            destination(destination), outputType(outputType) {}
};

typedef std::unique_ptr<CoinbaseOutput> CoinbaseOutputRef;

#endif
