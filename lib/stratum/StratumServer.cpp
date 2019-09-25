#include <memory>

#include "StratumServer.h"
#include "StratumJob.h"
#include "StratumClientConnection.h"


StratumServer::StratumServer(Listener &&listener) : ConnectionManager(_listener),
    extraNonce2Size(4),
    _listener(std::move(listener)),
    jobCounter(0)
{
}

std::unique_ptr<ConnectionManager::Connection> StratumServer::makeConnection(SocketBase &&socket, ConnectionManager &manager)
{
    return std::make_unique<StratumClientConnection>(manager, std::move(socket), this);
}

std::unique_ptr<StratumJob> StratumServer::createJob(StratumClientConnection *client)
{
    OBTAIN_LOCK(_jobGeneratorLock);

    return std::make_unique<StratumJob>(++this->jobCounter, 1.0);
}
