#ifndef DAEMONCONNECTOR_H
#define DAEMONCONNECTOR_H

#include <memory>

#include <bitcoinrpc/RPCConnection.h>
#include <stratum/StratumServer.h>
#include <stratum/BlockSubmitter.h>


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

        using RPCConnection::RPCConnection;

        NetworkStateRef getNetworkState(void);

        inline BlockSubmitterRef blockSubmitter(void)
        {
            return std::make_unique<BlockSubmitter>(*this);
        }

    private:
        NetworkStateRef getNetworkState(const BlockTemplate &rpcBlockTemplate) const;
};

typedef std::shared_ptr<DaemonConnector> DaemonConnectorRef;

#endif
