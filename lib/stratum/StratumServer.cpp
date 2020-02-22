#include <memory>

#include "StratumServer.h"
#include "StratumJob.h"
#include "StratumClientConnection.h"

#include <mining/CoinbaseWitnessCommitment.h>


StratumServer::StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, BlockSubmitterRef &&blockSubmitter, HashPluginRef hasher, const std::string &coinbaseId, double defaultDiff) : ConnectionManager(_listener),
    extraNonce2Size(sizeof(CoinbaseTransaction::nonce2_t)),
    _listener(std::move(listener)),
    hasher(hasher),
    _blockSubmitter(std::move(blockSubmitter)),
    coinbaseId(coinbaseId),
    defaultDiff(defaultDiff),
    jobCounter(0)
{
    /* Reserve space for witnessCommitment */
    this->coinbaseOutputs.resize(1);

    initializer.populateCoinbaseOutputs(this->coinbaseOutputs);

    /* Initialize server */
    this->updateNetworkState(initializer.getNetworkState());
}

std::unique_ptr<ConnectionManager::Connection> StratumServer::makeConnection(SocketBase &&socket, ConnectionManager &manager)
{
    return std::make_unique<StratumClientConnection>(manager, std::move(socket), this);
}

std::unique_ptr<StratumJob> StratumServer::createJob(StratumClientConnection *client)
{
    double diff = client->diff();

    if (diff < 0.0)
        diff = this->defaultDiff;

    OBTAIN_LOCK(_jobGeneratorLock);

    return std::make_unique<StratumJob>(++this->jobCounter, diff, this->state, this->coinbase, this->transactionsToInclude, this->merkleBranch, this->hasher);
}

void StratumServer::updateNetworkState(const NetworkStateRef &state)
{
    {
        OBTAIN_LOCK(_jobGeneratorLock);

        this->state = state;

        this->transactionsToInclude = std::make_shared<RawTransactions>();
        this->merkleBranch = std::make_shared<MerkleBranch>();
        this->witnessCommitment = std::make_shared<CoinbaseWitnessCommitment>(CoinbaseWitnessCommitment::forCoinbaseOnly());

        this->coinbaseOutputs[0] = this->witnessCommitment;
        this->coinbaseOutputs.rebalance(this->state->coinbaseCoins);
    }

    this->updateWork(true);
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
    {
        OBTAIN_LOCK(_jobGeneratorLock);
        this->_regenerateCoinbaseTransaction();
    }

    this->sendNewJobsToClients(forceClientUpdates);
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

void StratumServer::_regenerateCoinbaseTransaction()
{
    this->coinbase = std::make_shared<CoinbaseTransaction>(CoinbaseTransaction::build(
        this->state->blockHeight,
        this->coinbaseId,
        this->coinbaseOutputs
    ));
}
