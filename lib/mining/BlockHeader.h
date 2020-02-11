#ifndef MINING_BLOCKHEADER_H
#define MINING_BLOCKHEADER_H

#include <interfaces/plugins/HashPlugin.h>

#include "BlockHash.h"
#include "block.h"


class BlockHeader
{
    public:
        BlockHeader(uint32_t versionBits, const Hash256 &parentBlockhash, const Hash256 &merkleRoot, uint32_t time, uint32_t difficultyBits, uint32_t nonce);
        BlockHeader(const ConstByteStringRef &raw);

        BlockHash hash(const HashPluginRef &hasher) const;
        operator const ByteString(void) const;

        inline const blockheader &asCStruct(void) const         { return this->raw; }

    private:
        blockheader raw;
};

#endif
