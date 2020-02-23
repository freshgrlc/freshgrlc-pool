#include <memory>

#include "StratumServer.h"
#include "StratumJob.h"
#include "StratumClientConnection.h"

#include <mining/CoinbaseWitnessCommitment.h>

#include <util/logger.h>


StratumServer::StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, BlockSubmitterRef &&blockSubmitter, HashPluginRef hasher, const std::string &coinbaseId, double defaultDiff) : ListeningConnectionManager(_listener),
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

IncomingConnectionRef StratumServer::makeConnection(SocketBase &&socket)
{
    return std::make_unique<StratumClientConnection>(*this, std::move(socket));
}

std::unique_ptr<StratumJob> StratumServer::createJob(StratumClientConnection *client, double diff)
{
    if (diff < 0.0)
        diff = client->diff();

    if (diff < 0.0)
        diff = this->defaultDiff;

    OBTAIN_LOCK(_jobGeneratorLock);

    if (this->state->miningDiff * 0x100 < diff)
        diff = this->state->miningDiff * 0x100 / 4;

    return std::make_unique<StratumJob>(++this->jobCounter, diff, this->state, this->coinbase, this->transactionsToInclude, this->merkleBranch, this->hasher);
}

void StratumServer::updateNetworkState(const NetworkStateRef &state)
{
    mlog(INFO, "New block height %d, diff %.3f, %d transactions", state->blockHeight, state->miningDiff, 0);

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
    OBTAIN_FOREIGN_LOCK(this->connections().lock(), clients_lock);

    for (auto &client : this->connections().getAs<StratumClientConnection>())
    {
        if (forceUpdate)
            client->clearJobs();

        client->sendJob(forceUpdate);
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
