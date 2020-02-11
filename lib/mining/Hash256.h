#ifndef MINING_HASH256_H
#define MINING_HASH256_H

#include "Hash.h"

typedef uint8_t uint256_t[32];

class Hash256 : public Hash<uint256_t>
{
    public:
        Hash256(const Hash256 &src);
        Hash256(const uint256_t raw);
        Hash256(const ByteString &src);
        Hash256(const ConstByteStringRef &src);

        bool operator<(const Hash256 &them) const;
        bool operator<=(const Hash256 &them) const;

        static Hash256 zero(void);

    private:
        using Hash::Hash;
};

#endif
