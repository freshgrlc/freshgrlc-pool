#include "DaemonConnector.h"

DaemonConnector::DaemonConnector(const std::string &username, const std::string &password, const std::string &host, int port) : RPCConnection(username, password, host, port),
    _poller(*this),
    _blockSubmitter(*this),
    _stratumInitializer(*this),
    _stratum(nullptr)
{
}

NetworkState DaemonConnector::getNetworkState()
{
    return this->getNetworkState(this->getBlockTemplate());
}

NetworkState DaemonConnector::getNetworkState(const BlockTemplate &rpcBlockTemplate) const
{
    return {
        rpcBlockTemplate.version,
        rpcBlockTemplate.miningTargetBits,
        (uint32_t) rpcBlockTemplate.notBefore,
        rpcBlockTemplate.height,
        rpcBlockTemplate.previousBlockHash,
        rpcBlockTemplate.miningTarget,
        rpcBlockTemplate.coinbaseCoins
    };
}

void DaemonConnector::registerUpdateNotificationsFor(StratumServer &stratumServer)
{
    _stratum = &stratumServer;
    _poller.start();
}

void DaemonConnector::updateStratumServer(NetworkState &&newNetworkState)
{
    _stratum->updateNetworkState(std::move(newNetworkState));
}
