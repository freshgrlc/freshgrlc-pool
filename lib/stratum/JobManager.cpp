#include "JobManager.h"
#include "StratumServer.h"
#include "StratumClientConnection.h"
#include "StratumConnectionManager.h"
#include "StratumJob.h"


JobManager::JobManager(StratumServer &server, StratumConnectionManager &connectionManager, double defaultDiff) :
    _server(server),
    _connectionManager(connectionManager),
    defaultDiff(defaultDiff),
    jobCounter(0)
{
}

std::unique_ptr<StratumJob> JobManager::createJob(StratumClientConnection *client, double diff)
{
    if (diff < 0.0)
        diff = client->diff();

    if (diff < 0.0)
        diff = this->defaultDiff;

    OBTAIN_LOCK(_lock);

    const NetworkState &networkState = this->generationInformation->networkState;

    if (networkState.miningDiff * 0x100 < diff)
        diff = networkState.miningDiff * 0x100 / 4;

    return std::make_unique<StratumJob>(++this->jobCounter, diff, this->generationInformation, this->coinbase, this->server().hasher());
}

void JobManager::updateWork(const BlockGenerationInformationRef &generationInformation)
{
    bool blockUpdate = generationInformation->networkState != this->generationInformation->networkState;

    {
        OBTAIN_LOCK(_lock);
        this->generationInformation = generationInformation;
    }

    this->updateCoinbaseOutputs();
    this->updateWork(blockUpdate);
}

void JobManager::updateCoinbaseOutputs(CoinbaseOutputs &&newOutputs)
{
    this->coinbaseOutputs = CoinbaseOutputs(std::move(newOutputs));

    /* Reserve room for witness commitment */
    this->coinbaseOutputs.emplace(this->coinbaseOutputs.begin(), nullptr);

    this->updateCoinbaseOutputs();
    this->updateWork();
}

void JobManager::updateCoinbaseOutputs()
{
    this->coinbaseOutputs[0] = std::make_unique<CoinbaseWitnessCommitment>(this->generationInformation->witnessCommitment);
    this->coinbaseOutputs.rebalance(this->generationInformation->networkState.coinbaseCoins);

}

void JobManager::updateWork(bool forceClientUpdates)
{
    {
        OBTAIN_LOCK(_lock);
        this->coinbase = this->generateCoinbaseTransaction();
    }

    this->sendNewJobsToClients(forceClientUpdates);
}

void JobManager::sendNewJobsToClients(bool forceUpdate)
{
    OBTAIN_FOREIGN_LOCK(_connectionManager.lock(), clients_lock);

    for (auto &client : _connectionManager.connections())
    {
        if (forceUpdate)
            client->clearJobs();

        client->sendJob(forceUpdate);
    }
}

CoinbaseTransactionRef JobManager::generateCoinbaseTransaction()
{
    /* Insert/update witness commitment, always first output */
    AbstractCoinbaseOutputRef newWitnessCommitment = std::make_unique<CoinbaseCommitment>(this->generationInformation->witnessCommitment);

    if (this->coinbaseOutputs.size() == 0 || !CoinbaseCommitment::isCommitment(*this->coinbaseOutputs[0]))
        this->coinbaseOutputs.emplace(this->coinbaseOutputs.begin(), std::move(newWitnessCommitment));
    else
        this->coinbaseOutputs[0] = std::move(newWitnessCommitment);

    this->coinbaseOutputs.rebalance(this->generationInformation->networkState.coinbaseCoins);

    return std::make_shared<CoinbaseTransaction>(CoinbaseTransaction::build(
        this->generationInformation->networkState.blockHeight,
        _server.coinbaseId(),
        this->coinbaseOutputs
    ));
}
