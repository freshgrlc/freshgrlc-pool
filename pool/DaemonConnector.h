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

                inline void submitBlock(const BlockHeader &header, const ByteString &coinbaseTransaction, RawTransactionsRef otherTransactions) override
                {
                    this->parent.submitBlock(header, coinbaseTransaction, *otherTransactions);
                }

            private:
                DaemonConnector &parent;
        };

        class StratumInitializer : public StratumServer::NetworkStateInitializer
        {
            public:
                inline StratumInitializer(DaemonConnector &parent) : parent(parent) {}

                inline NetworkStateRef getNetworkState(void) const override
                {
                    return parent.getNetworkState();
                }

            private:
                DaemonConnector &parent;
        };

        typedef std::unique_ptr<BlockSubmitter> BlockSubmitterRef;


        DaemonConnector(const std::string &username, const std::string &password, const std::string &host, int port);

        NetworkStateRef getNetworkState(void);
        void updateStratumServers(const NetworkStateRef &newNetworkState);

        inline BlockSubmitterRef blockSubmitter(void)
        {
            return std::make_unique<BlockSubmitter>(*this);
        }

        inline void registerServer(StratumServer &stratumServer)
        {
            this->servers.push_back(&stratumServer);
        }

    private:
        BlockTemplatePoller poller;
        std::vector<StratumServer *> servers;

        NetworkStateRef getNetworkState(const BlockTemplate &rpcBlockTemplate) const;

        friend class BlockTemplatePoller;
};

typedef std::shared_ptr<DaemonConnector> DaemonConnectorRef;

#endif
