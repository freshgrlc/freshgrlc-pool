#ifndef MINING_HASH160_H
#define MINING_HASH160_H

#include "Hash.h"

typedef uint8_t uint160_t[20];

class Hash160 : public Hash<uint160_t>
{
    public:
        Hash160(const Hash160 &src);
        Hash160(const uint160_t raw);
        Hash160(const ByteString &src);
        Hash160(const ConstByteStringRef &src);

        inline bool operator<(const Hash160 &them) const;

        static Hash160 zero(void);

    private:
        using Hash::Hash;
};

#endif
