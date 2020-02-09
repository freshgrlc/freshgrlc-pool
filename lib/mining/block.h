#ifndef MINING_BLOCK_H
#define MINING_BLOCK_H

#include <stdint.h>

#include "Hash256.h"

typedef struct _blockheader
{
    int32_t     versionBits;
    uint256_t   parentBlockHash;
    uint256_t   merkleRootHash;
    uint32_t    timestamp;
    uint32_t    difficultyBits;
    uint32_t    nonce;
} blockheader;

#endif
