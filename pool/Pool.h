#ifndef POOL_H
#define POOL_H

#include "DaemonConnector.h"
#include "StratumConfiguration.h"

#include <util/Thread.h>


class Pool
{
    public:
        Pool(DaemonConnectorRef &daemon, const DaemonConnector::StratumInitializer &stratumInitializer, const StratumConfiguration &configuration);

    private:
        class StratumListener : public Thread
        {
            public:
                inline StratumListener(StratumServer &stratumServer) : stratumServer(stratumServer) {}
                void main(void) override;

            private:
                StratumServer &stratumServer;
        };

        DaemonConnectorRef daemon;
        StratumServer stratum;
        StratumListener stratumListener;
};

#endif
