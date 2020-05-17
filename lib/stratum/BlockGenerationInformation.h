#ifndef STRATUM_BLOCKTEMPLATEINFO_H
#define STRATUM_BLOCKTEMPLATEINFO_H

#include <mining/RawTransactions.h>
#include <mining/CoinbaseWitnessCommitment.h>
#include <mining/MerkleBranch.h>

#include "NetworkState.h"


class BlockGenerationInformation
{
    public:
        const NetworkState networkState;
        const RawTransactions transactions;
        const CoinbaseWitnessCommitment witnessCommitment;
        const MerkleBranch merkleBranch;

        BlockGenerationInformation(NetworkState &&networkState);
        BlockGenerationInformation(BlockGenerationInformation &&src);
};


#include <memory>

typedef std::shared_ptr<BlockGenerationInformation> BlockGenerationInformationRef;

#endif
