#ifndef STRATUM_STRATUMSERVER_H
#define STRATUM_STRATUMSERVER_H

#include <memory>

#include <socket/ListeningConnectionManager.h>
#include <mining/CoinbaseOutputs.h>
#include <mining/MerkleBranch.h>
#include <interfaces/plugins/HashPlugin.h>
#include <util/Thread.h>

#include "NetworkState.h"
#include "BlockSubmitter.h"
#include "BlockGenerationInformation.h"
#include "Pool.h"


class StratumConnectionManager;

class StratumServer : public ListeningConnectionManager
{
    public:
        class NetworkStateInitializer
        {
            public:
                virtual NetworkState getNetworkState(void) const = 0;
        };

        StratumServer(Listener &&listener, const NetworkStateInitializer &initializer, BlockSubmitter &blockSubmitter, const HashPluginRef hasher, const std::string &coinbaseId);

        void updateNetworkState(NetworkState &&state);

        inline BlockSubmitter &blockSubmitter(void) const                       { return _blockSubmitter; }
        inline BlockGenerationInformationRef blockGenerationInformation(void)   { return _generationInformation; }
        inline HashPluginRef hasher(void) const                                 { return _hasher; }
        inline const ByteString coinbaseId(void) const                          { return _coinbaseId; }

    protected:
        friend class StratumConnectionManager;

        virtual IncomingConnectionRef makeConnection(SocketBase &&socket) override;

    private:
        HashPluginRef _hasher;
        BlockSubmitter &_blockSubmitter;

        ByteString _coinbaseId;

        BlockGenerationInformationRef _generationInformation;

        std::vector<Pool> pools;
};

#endif
