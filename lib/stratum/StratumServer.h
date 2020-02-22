#ifndef STRATUM_STRATUMSERVER_H
#define STRATUM_STRATUMSERVER_H

#include <memory>

#include <socket/ConnectionManager.h>
#include <mining/CoinbaseOutputs.h>
#include <mining/RawTransactions.h>
#include <mining/MerkleBranch.h>
#include <mining/CoinbaseTransaction.h>
#include <interfaces/plugins/HashPlugin.h>

#include "NetworkState.h"
#include "BlockSubmitter.h"

class StratumJob;
class StratumClientConnection;

class StratumServer : public ConnectionManager
{
    public:
        class NetworkStateInitializer
        {
            public:
                virtual NetworkStateRef getNetworkState(void) const = 0;
                virtual void populateCoinbaseOutputs(CoinbaseOutputs &outputs) const = 0;
        };

        const int extraNonce2Size;

        StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, BlockSubmitterRef &&blockSubmitter, const HashPluginRef hasher, const std::string &coinbaseId, double defaultDiff = 1.0);

        std::unique_ptr<StratumJob> createJob(StratumClientConnection *client);

        void updateNetworkState(const NetworkStateRef &state);

        void updateCoinbaseOutputs(const CoinbaseOutputs &newOutputs);
        void updateCoinbaseOutputs(CoinbaseOutputs &&newOutputs);

        inline BlockSubmitter &blockSubmitter(void) { return *_blockSubmitter; }

    private:
        Listener _listener;
        Lock _jobGeneratorLock;

        HashPluginRef hasher;
        BlockSubmitterRef _blockSubmitter;

        ByteString coinbaseId;

        NetworkStateRef state;
        RawTransactionsRef transactionsToInclude;
        MerkleBranchRef merkleBranch;
        AbstractCoinbaseOutputRef witnessCommitment;

        CoinbaseOutputs coinbaseOutputs;
        CoinbaseTransactionRef coinbase;

        double defaultDiff;
        uint32_t jobCounter;

        void updateWork(bool forceClientUpdates = false);
        void sendNewJobsToClients(bool forceUpdate);

        void _regenerateCoinbaseTransaction(void);

        virtual std::unique_ptr<Connection> makeConnection(SocketBase &&socket, ConnectionManager &manager) override;
};

#endif
