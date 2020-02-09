#ifndef STRATUM_STRATUMSERVER_H
#define STRATUM_STRATUMSERVER_H

#include <memory>

#include <socket/ConnectionManager.h>
#include <mining/CoinbaseOutputs.h>
#include <mining/MerkleBranch.h>
#include <mining/CoinbaseTransaction.h>

#include "NetworkState.h"

class StratumJob;
class StratumClientConnection;

class StratumServer : public ConnectionManager
{
    public:
        class NetworkStateInitializer
        {
            public:
                virtual NetworkStateRef getNetworkState(void) const = 0;
                virtual void populateCoinbaseOutputs(CoinbaseOutputs outputs) const = 0;
        };

        const int extraNonce2Size;

        StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, const std::string &coinbaseId);

        std::unique_ptr<StratumJob> createJob(StratumClientConnection *client);

        void updateNetworkState(const NetworkState &state);
        void updateCoinbaseOutputs(const CoinbaseOutputs &newOutputs);
        void updateCoinbaseOutputs(CoinbaseOutputs &&newOutputs);
        void updateMempool();

    private:
        Listener _listener;
        Lock _jobGeneratorLock;

        ByteString coinbaseId;

        NetworkStateRef state;
        MerkleBranchRef merkleBranch;
        AbstractCoinbaseOutputRef witnessCommitment;

        CoinbaseOutputs coinbaseOutputs;
        CoinbaseTransactionRef coinbase;

        uint32_t jobCounter;

        void updateWork(bool forceClientUpdates = false);
        void regenerateCoinbaseTransaction(void);
        void _regenerateCoinbaseTransaction(void);
        void sendNewJobsToClients(bool forceUpdate);

        virtual std::unique_ptr<Connection> makeConnection(SocketBase &&socket, ConnectionManager &manager) override;
};

#endif
