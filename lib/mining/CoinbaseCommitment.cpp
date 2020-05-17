#include "CoinbaseCommitment.h"

using namespace script;

const Script CoinbaseCommitment::redeemScript() const
{
    return Script() << OP_RETURN << this->commitment;
}

bool CoinbaseCommitment::isCommitment(const AbstractCoinbaseOutput &output)
{
    return output.redeemScript()[0] == OP_RETURN;
}
