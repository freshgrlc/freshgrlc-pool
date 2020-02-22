#include "DaemonConnector.h"

NetworkStateRef DaemonConnector::getNetworkState()
{
    return this->getNetworkState(this->getBlockTemplate());
}

NetworkStateRef DaemonConnector::getNetworkState(const BlockTemplate &rpcBlockTemplate) const
{
    return std::make_shared<NetworkState>(
        rpcBlockTemplate.version,
        rpcBlockTemplate.miningTargetBits,
        rpcBlockTemplate.notBefore,
        rpcBlockTemplate.height,
        rpcBlockTemplate.previousBlockHash,
        rpcBlockTemplate.miningTarget,
        rpcBlockTemplate.coinbaseCoins
    );
}


