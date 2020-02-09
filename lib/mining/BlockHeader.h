#ifndef MINING_BLOCKHEADER_H
#define MINING_BLOCKHEADER_H

#include <interfaces/plugins/HashPlugin.h>

#include "BlockHash.h"


class BlockHeader
{
    public:
        BlockHash hash(HashPlugin hasher);
};

#endif
