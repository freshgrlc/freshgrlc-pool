#include "CoinbaseTransaction.h"

#include "VarInt.h"
#include "VarData.h"
#include "Hash256.h"
#include "txscript/PushData.h"

#include <ctime>


ConstByteStringRef CoinbaseTransaction::part1()
{
    return ConstByteStringRef(&this->raw[0], this->part1Size);
}

ConstByteStringRef CoinbaseTransaction::part2()
{
    return ConstByteStringRef(&this->raw[part2Start], this->raw.size() - this->part2Start);
}

CoinbaseTransaction CoinbaseTransaction::build(uint32_t height, const ConstByteStringRef &identification, const CoinbaseOutputs &outputs)
{
    return build(height, 0, 0, identification, outputs);
}

CoinbaseTransaction CoinbaseTransaction::build(uint32_t height, uint32_t nonce1, uint32_t nonce2, const ConstByteStringRef &identification, const CoinbaseOutputs &outputs)
{
    ByteString raw;
    size_t part1Size = 0;
    size_t part2Start = 0;
    size_t scriptSize = 0;

    static const uint32_t VERSION = 1;

    raw.appendLE(VERSION);
    raw += VarInt(1);                       // inputs

    raw += Hash256::zero();                 // input: previous tx
    raw.appendLE((uint32_t) -1);            // input: previous tx output index

    script::Script redeemScript = makeCoinbaseRedeemScript(height, nonce1, nonce2, identification, part1Size, part2Start);
    scriptSize = redeemScript.size();

    raw += VarData::encode(redeemScript);   // input: script

    part1Size += raw.size() - scriptSize;
    part2Start += raw.size() - scriptSize;

    raw.appendLE((uint32_t) 0);             // input: sequence nr


    raw += VarInt(outputs.size());          // outputs
    for (auto &output : outputs)
        raw += *output;

    raw.appendLE((uint32_t) 0);             // locktime

    return CoinbaseTransaction(std::move(raw), part1Size, part2Start);
}

script::Script CoinbaseTransaction::makeCoinbaseRedeemScript(uint32_t height, uint32_t nonce1, uint32_t nonce2, const ConstByteStringRef &identification, size_t &part1Size, size_t &part2Start)
{
    using namespace script;

    Script raw = Script() << (PushData) height << (PushData) (uint32_t) std::time(NULL);

    part1Size = raw.size() + 1;     /* Reserve room for len field */

    raw = raw << ByteString().appendLE(nonce1).appendLE(nonce2);

    part2Start = raw.size();

    return raw << identification;
}


