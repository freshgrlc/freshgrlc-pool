#ifndef DAEMONCONNECTOR_H
#define DAEMONCONNECTOR_H

#include <memory>

#include <bitcoinrpc/RPCConnection.h>
#include <stratum/StratumServer.h>
#include <stratum/BlockSubmitter.h>

#include "BlockTemplatePoller.h"


class DaemonConnector : public RPCConnection
{
    public:
        class BlockSubmitter : public ::BlockSubmitter
        {
            public:
                inline BlockSubmitter(DaemonConnector &parent) : parent(parent) {}
                inline ~BlockSubmitter(void) override {}

                inline void submitBlock(const BlockHeader &header, const ByteString &coinbaseTransaction, const RawTransactions &otherTransactions) override
                {
                    this->parent.submitBlock(header, coinbaseTransaction, otherTransactions);
                }

            private:
                DaemonConnector &parent;
        };

        class StratumInitializer : public StratumServer::NetworkStateInitializer
        {
            public:
                inline StratumInitializer(DaemonConnector &parent) : parent(parent) {}

                inline NetworkState getNetworkState(void) const override
                {
                    return parent.getNetworkState();
                }

            private:
                DaemonConnector &parent;
        };

        typedef std::unique_ptr<BlockSubmitter> BlockSubmitterRef;


        DaemonConnector(const std::string &username, const std::string &password, const std::string &host, int port);

        void registerUpdateNotificationsFor(StratumServer &stratumServer);

        inline BlockSubmitter &blockSubmitter(void)             { return _blockSubmitter; }
        inline StratumInitializer &stratumInitializer(void)     { return _stratumInitializer; }

    protected:
        friend class BlockTemplatePoller;

        void updateStratumServer(NetworkState &&newNetworkState);

    private:
        BlockTemplatePoller _poller;
        BlockSubmitter _blockSubmitter;
        StratumInitializer _stratumInitializer;
        StratumServer *_stratum;

        NetworkState getNetworkState(void);
        NetworkState getNetworkState(const BlockTemplate &rpcBlockTemplate) const;

        friend class BlockTemplatePoller;
};

typedef std::shared_ptr<DaemonConnector> DaemonConnectorRef;

#endif
