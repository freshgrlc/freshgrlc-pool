#include <memory>

#include "StratumServer.h"
#include "StratumJob.h"
#include "StratumClientConnection.h"

#include <mining/CoinbaseWitnessCommitment.h>

#include <util/logger.h>


StratumServer::StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, BlockSubmitter &blockSubmitter, HashPluginRef hasher, const std::string &coinbaseId) : ListeningConnectionManager(std::move(listener)),
    _hasher(hasher),
    _blockSubmitter(blockSubmitter),
    _coinbaseId(coinbaseId)
{
    /* Initialize server */
    this->updateNetworkState(initializer.getNetworkState());
}

IncomingConnectionRef StratumServer::makeConnection(SocketBase &&socket)
{
    return std::make_unique<StratumClientConnection>(*this, std::move(socket));
}

void StratumServer::updateNetworkState(NetworkState &&state)
{
    mlog(INFO, "New block height %d, diff %.3f, %d transactions", state.blockHeight, state.miningDiff, 0);

    _generationInformation = std::make_shared<BlockGenerationInformation>(std::move(state));
}



