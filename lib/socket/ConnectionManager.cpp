#include "ConnectionManager.h"
#include "IncomingConnection.h"

#include <util.h>

#include <unistd.h>
#include <ctime>


ConnectionManager::ConnectionManager() : connectionsHolder(std::make_shared<Connections>())
{
}

void ConnectionManager::addConnection(SocketBase &&socket)
{
    IncomingConnectionRef newConnection = this->makeConnection(std::move(socket));

    this->connections().add(std::move(newConnection)).enableProcessing();
}

void ConnectionManager::addConnection(SocketBase &&socket, void *manager)
{
    ((ConnectionManager *) manager)->addConnection(std::move(socket));
}

IncomingConnection &ConnectionManager::Connections::add(IncomingConnectionRef &&connection)
{
    OBTAIN_LOCK(_lock);

    _clients.push_back(std::move(connection));

    mlog(DEBUG, "Added connection, new total: %ld", _clients.size());

    return *_clients.end()[-1];
}

IncomingConnectionRef ConnectionManager::Connections::remove(IncomingConnection &connection, bool take)
{
    OBTAIN_LOCK(_lock);

    for (auto iter = _clients.begin(); iter != _clients.end(); iter++)
    {
        if (iter->get()->dangling())
        {
            mlog(DEBUG, "Removing old dangling connection %ld", iter - _clients.begin());
            iter = _clients.erase(iter);
        }

        if (iter->get() == &connection)
        {
            IncomingConnectionRef retval(take ? std::move(*iter) : nullptr);

            _clients.erase(iter);
            mlog(DEBUG, "Removed connection %ld", iter - _clients.begin());

            return retval;
        }
    }

    return nullptr;
}
