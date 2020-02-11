#include "RawTransactions.h"

#include "VarInt.h"

RawTransactions::operator ByteString(void)
{
    ByteString raw;

    raw += VarInt(this->size());

    for (auto &tx : *this)
        raw += tx;

    return raw;
}
