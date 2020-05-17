#include "StratumConnectionManager.h"
#include "StratumServer.h"
#include "StratumPool.h"

#include <socket/IncomingConnection.h>

IncomingConnectionRef StratumConnectionManager::makeConnection(SocketBase &&socket)
{
    return this->parent.server().makeConnection(std::move(socket));
}
