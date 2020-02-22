#include "DaemonConnector.h"

DaemonConnector::DaemonConnector(const std::string &username, const std::string &password, const std::string &host, int port) : RPCConnection(username, password, host, port),
    poller(*this)
{
    this->poller.start();
}

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

void DaemonConnector::updateStratumServers(const NetworkStateRef &newNetworkState)
{
    for (auto server : this->servers)
        server->updateNetworkState(newNetworkState);
}
