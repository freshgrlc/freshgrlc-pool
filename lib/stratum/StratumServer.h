#ifndef STRATUM_STRATUMSERVER_H
#define STRATUM_STRATUMSERVER_H

#include <memory>

#include <socket/ConnectionManager.h>


class StratumJob;
class StratumClientConnection;

class StratumServer : public ConnectionManager
{
    public:
        const int extraNonce2Size;

        StratumServer(Listener &&listener);

        std::unique_ptr<StratumJob> createJob(StratumClientConnection *client);

    private:
        Listener _listener;
        Lock _jobGeneratorLock;

        uint32_t jobCounter;

        virtual std::unique_ptr<Connection> makeConnection(SocketBase &&socket, ConnectionManager &manager) override;
};

#endif
