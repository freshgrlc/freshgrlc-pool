#include "CoinbaseWitnessCommitment.h"

#include "hashplugin.h"


const uint8_t CoinbaseWitnessCommitment::MAGIC[4] =
{
    0xaa, 0x21, 0xa9, 0xed
};

static ConstByteStringRef stripOpReturn(const ConstByteStringRef &rawData)
{
    if (rawData.length >= 2 && rawData[0] == script::OP_RETURN)
        return ConstByteStringRef(rawData.data + 2, rawData.length - 2);

    return rawData;
}

CoinbaseWitnessCommitment::CoinbaseWitnessCommitment(const ConstByteStringRef &rawData) : CoinbaseCommitment(stripOpReturn(rawData))
{
}

CoinbaseWitnessCommitment::CoinbaseWitnessCommitment(const Hash256 &hash) : CoinbaseCommitment(ByteString(MAGIC) + hash)
{
}

CoinbaseWitnessCommitment::CoinbaseWitnessCommitment(const script::Script &redeemScript) : CoinbaseCommitment(stripOpReturn(redeemScript))
{
}

CoinbaseWitnessCommitment CoinbaseWitnessCommitment::forCoinbaseOnly()
{
    Hash256 coinbaseWitness = Hash256::zero();
    Hash256 commitment = get_hashplugin("sha256d")->hash(ByteString() + coinbaseWitness + coinbaseWitness);

    return CoinbaseWitnessCommitment(commitment);
}
