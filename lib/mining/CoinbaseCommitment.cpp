#include "CoinbaseCommitment.h"

using namespace script;

const Script CoinbaseCommitment::redeemScript()
{
    return Script() << OP_RETURN << this->commitment;
}
