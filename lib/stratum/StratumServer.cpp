#include <memory>

#include "StratumServer.h"
#include "StratumJob.h"
#include "StratumClientConnection.h"

#include <mining/CoinbaseWitnessCommitment.h>


StratumServer::StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, const std::string &coinbaseId) : ConnectionManager(_listener),
    extraNonce2Size(4),
    _listener(std::move(listener)),
    coinbaseId(coinbaseId),
    jobCounter(0)
{
    /* Initialize server */
    OBTAIN_LOCK(_jobGeneratorLock);

    this->state = initializer.getNetworkState();
    this->witnessCommitment = std::make_shared<CoinbaseWitnessCommitment>(CoinbaseWitnessCommitment::forCoinbaseOnly());
    this->merkleBranch = std::make_shared<MerkleBranch>();

    this->coinbaseOutputs.push_back(this->witnessCommitment);
    initializer.populateCoinbaseOutputs(this->coinbaseOutputs);
    this->coinbaseOutputs.rebalance(this->state->coinbaseCoins);
    this->_regenerateCoinbaseTransaction();
}

std::unique_ptr<ConnectionManager::Connection> StratumServer::makeConnection(SocketBase &&socket, ConnectionManager &manager)
{
    return std::make_unique<StratumClientConnection>(manager, std::move(socket), this);
}

std::unique_ptr<StratumJob> StratumServer::createJob(StratumClientConnection *client)
{
    OBTAIN_LOCK(_jobGeneratorLock);

    return std::make_unique<StratumJob>(++this->jobCounter, client->diff(), this->state, this->coinbase, this->merkleBranch);
}

void StratumServer::updateCoinbaseOutputs(const CoinbaseOutputs &newOutputs)
{
    this->coinbaseOutputs.clear();

    this->coinbaseOutputs.push_back(this->witnessCommitment);
    this->coinbaseOutputs.insert(this->coinbaseOutputs.end(), newOutputs.begin(), newOutputs.end());

    this->coinbaseOutputs.rebalance(this->state->coinbaseCoins);
    this->updateWork();
}

void StratumServer::updateCoinbaseOutputs(CoinbaseOutputs &&newOutputs)
{
    this->coinbaseOutputs = CoinbaseOutputs(std::move(newOutputs));

    this->coinbaseOutputs.insert(this->coinbaseOutputs.begin(), 1, this->witnessCommitment);

    this->coinbaseOutputs.rebalance(this->state->coinbaseCoins);
    this->updateWork();
}

void StratumServer::updateWork(bool forceClientUpdates)
{
    this->regenerateCoinbaseTransaction();
    this->sendNewJobsToClients(forceClientUpdates);
}

void StratumServer::regenerateCoinbaseTransaction()
{
    OBTAIN_LOCK(_jobGeneratorLock);
    this->_regenerateCoinbaseTransaction();
}

void StratumServer::_regenerateCoinbaseTransaction()
{
    this->coinbase = std::make_shared<CoinbaseTransaction>(CoinbaseTransaction::build(
        this->state->blockHeight,
        this->coinbaseId,
        this->coinbaseOutputs
    ));
}

void StratumServer::sendNewJobsToClients(bool forceUpdate)
{
    OBTAIN_LOCK(_clients_lock);

    for (auto &client : this->clients())
    {
        StratumClientConnection &stratumClient = *((StratumClientConnection *) &*client);

        if (forceUpdate)
            stratumClient.clearJobs();

        stratumClient.sendJob(forceUpdate);
    }
}


