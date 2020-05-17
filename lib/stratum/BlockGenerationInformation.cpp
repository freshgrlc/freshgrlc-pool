#include "BlockGenerationInformation.h"

#include <mining/CoinbaseWitnessCommitment.h>


BlockGenerationInformation::BlockGenerationInformation(NetworkState &&networkState) :
    networkState(std::move(networkState)),
    transactions(),
    witnessCommitment(CoinbaseWitnessCommitment::forCoinbaseOnly()),
    merkleBranch()
{
}

BlockGenerationInformation::BlockGenerationInformation(BlockGenerationInformation &&src) :
    networkState(std::move(src.networkState)),
    transactions(std::move(src.transactions)),
    witnessCommitment(std::move(src.witnessCommitment)),
    merkleBranch(std::move(src.merkleBranch))
{
}
