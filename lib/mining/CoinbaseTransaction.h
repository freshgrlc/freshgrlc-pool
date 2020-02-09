#ifndef MINING_COINBASETRANSACTION_H
#define MINING_COINBASETRANSACTION_H

#include <util/ByteString.h>

#include "CoinbaseOutputs.h"
#include "txscript/Script.h"

class CoinbaseTransaction
{
    public:
        typedef uint32_t nonce1_t;
        typedef uint32_t nonce2_t;

        static CoinbaseTransaction build(uint32_t height, const ConstByteStringRef &identification, const CoinbaseOutputs &outputs);
        static CoinbaseTransaction build(uint32_t height, nonce1_t nonce1, nonce2_t nonce2, const ConstByteStringRef &identification, const CoinbaseOutputs &outputs);

        ConstByteStringRef part1(void);
        ConstByteStringRef part2(void);

    private:
        inline CoinbaseTransaction(ByteString &&raw, size_t part1Size, size_t part2Start) :
            raw(raw), part1Size(part1Size), part2Start(part2Start)
        {}

        const ByteString raw;
        const size_t part1Size;
        const size_t part2Start;

        static script::Script makeCoinbaseRedeemScript(uint32_t height, nonce1_t nonce1, nonce2_t nonce2, const ConstByteStringRef &identification, size_t &part1Size, size_t &part2Start);
};

#include <memory>

typedef std::shared_ptr<CoinbaseTransaction> CoinbaseTransactionRef;

#endif
