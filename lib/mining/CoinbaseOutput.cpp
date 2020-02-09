#include "CoinbaseOutput.h"

#include "txscript/Script.h"

#include "VarData.h"

using namespace script;


const Script CoinbaseOutput::redeemScript()
{
    switch (this->outputType)
    {
        case P2PKH:
        default:
            return Script() << OP_DUP << OP_HASH160 << this->destination << OP_EQUALVERIFY << OP_CHECKSIG;

        case P2SH:
            return Script() << OP_HASH160 << this->destination << OP_EQUAL;
            break;

        case P2WPKH:
            return Script() << OP_0 << this->destination;
    }
}
