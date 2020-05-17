#ifndef STRATUM_STRATUMCONNECTIONMANAGER_H
#define STRATUM_STRATUMCONNECTIONMANAGER_H

#include <socket/ConnectionManager.h>

class Pool;
class StratumClientConnection;

class StratumConnectionManager : public ConnectionManager
{
    public:
        inline StratumConnectionManager(Pool &parent) : ConnectionManager(), parent(parent) {}

        inline auto &connections(void)      { return this->connectionsHolder->getAs<StratumClientConnection>(); }
        inline Lock &lock(void)             { return this->connectionsHolder->lock(); }

    private:
        Pool &parent;

        IncomingConnectionRef makeConnection(SocketBase &&socket) override;
};

#endif
