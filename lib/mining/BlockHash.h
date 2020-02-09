#ifndef MINING_BLOCKHASH_H
#define MINING_BLOCKHASH_H

#include "Hash256.h"

class BlockHash : public Hash256
{
    public:
        inline BlockHash(const Hash256 &hash) : Hash256(hash) {}
};

#endif
