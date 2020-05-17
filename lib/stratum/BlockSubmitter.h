#ifndef STRATUM_BLOCKSUBMITTER_H
#define STRATUM_BLOCKSUBMITTER_H

#include <mining/RawTransactions.h>

class BlockHeader;
class ByteString;

class BlockSubmitter
{
    public:
        inline virtual ~BlockSubmitter(void) {}
        virtual void submitBlock(const BlockHeader &header, const ByteString &coinbaseTransaction, const RawTransactions &otherTransactions) = 0;
};

#include <memory>

typedef std::unique_ptr<BlockSubmitter> BlockSubmitterRef;

#endif
