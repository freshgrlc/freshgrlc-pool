#ifndef STRATUM_JOBMANAGER_H
#define STRATUM_JOBMANAGER_H

#include <memory>

#include <mining/CoinbaseOutputs.h>
#include <mining/CoinbaseTransaction.h>
#include <util/Lock.h>

#include "NetworkState.h"
#include "BlockGenerationInformation.h"

class StratumJob;
class StratumClientConnection;
class StratumConnectionManager;
class StratumServer;

class JobManager
{
    public:
        const size_t extraNonce2Size;

        JobManager(StratumServer &server, StratumConnectionManager &connectionManager, double defaultDiff = 1.0);

        std::unique_ptr<StratumJob> createJob(StratumClientConnection *client, double diff = -1.0);

        void updateWork(const BlockGenerationInformationRef &generationInformation);
        void updateCoinbaseOutputs(CoinbaseOutputs &&newOutputs);

        inline StratumServer &server(void)          { return _server; }

    private:
        StratumServer &_server;
        StratumConnectionManager &_connectionManager;

        Lock _lock;

        BlockGenerationInformationRef generationInformation;
        CoinbaseOutputs coinbaseOutputs;
        CoinbaseTransactionRef coinbase;

        double defaultDiff;
        uint32_t jobCounter;

        void updateWork(bool forceClientUpdates = false);
        void sendNewJobsToClients(bool forceUpdate);

        void updateCoinbaseOutputs(void);

        CoinbaseTransactionRef generateCoinbaseTransaction(void);
};

#endif
