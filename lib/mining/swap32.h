#ifndef MINING_SWAP32_H
#define MINING_SWAP32_H

#include <stdint.h>
#include <stddef.h>

inline uint32_t readBE(uint8_t int32[4])
{
    return (int32[0] << 24) | (int32[1] << 16) | (int32[2] << 8) | int32[3];
}

inline uint32_t swap32(uint32_t x)
{
    return readBE((uint8_t *) &x);
}

inline void swap32(uint8_t int32[4])
{
    *((uint32_t *) int32) = readBE(int32);
}

inline void swap32(uint32_t *int32)
{
    swap32((uint8_t *) int32);
}

typedef uint8_t uint256_t[32];

inline void swap32_256(uint256_t &int256)
{
    for (size_t i = 0; i < sizeof(uint256_t); i += 4)
        swap32(&int256[i]);
}

#endif
