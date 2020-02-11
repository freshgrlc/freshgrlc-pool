#ifndef MINING_RAWTRANSACTION_H
#define MINING_RAWTRANSACTION_H

#include "Hash256.h"

class RawTransaction
{
    public:
        const Hash256 id;
        const ByteString raw;

        inline RawTransaction(const Hash256 &id, const ByteString &raw) : id(id), raw(raw) {}

        inline operator ConstByteStringRef(void) const  { return this->raw; }
};

#endif
