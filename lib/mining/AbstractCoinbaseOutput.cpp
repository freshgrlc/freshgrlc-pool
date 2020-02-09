#include "AbstractCoinbaseOutput.h"

#include "VarData.h"


AbstractCoinbaseOutput::operator const ByteString()
{
    ByteString encoded;

    encoded.appendLE(this->coins);
    encoded += VarData::encode(this->redeemScript());

    return encoded;
}
