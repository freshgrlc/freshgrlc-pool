#ifndef MINING_VARINT_H
#define MINING_VARINT_H

#include <util/ByteString.h>

class VarInt
{
    public:
        const uint64_t raw;

        inline VarInt(uint64_t x) : raw(x) {}

        ByteString encode(void);

        inline operator ByteString(void)    { return this->encode(); }
};

#endif
