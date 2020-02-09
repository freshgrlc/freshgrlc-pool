#ifndef MINING_VARDATA_H
#define MINING_VARDATA_H

#include <util/ByteString.h>

#include "VarInt.h"

class VarData : public ByteString
{
    public:
        using ByteString::ByteString;

        inline operator const ByteString(void)
        {
            ByteString encoded;

            encoded += VarInt(this->size());
            encoded += *this;

            return encoded;
        };

        inline static const ByteString encode(ConstByteStringRef constData)
        {
            ByteString encoded;

            encoded += VarInt(constData.length);
            encoded += constData;

            return encoded;
        }
};

#endif
