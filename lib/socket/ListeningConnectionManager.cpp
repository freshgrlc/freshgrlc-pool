#include "ListeningConnectionManager.h"

#include <util/logger.h>


int ListeningConnectionManager::listen()
{
    int result;

    if (!_listener.connected())
    {
        if ((result = _listener.open()))
            return result;
    }

    mlog(INFO, "Opened listener on port %d", _listener.port());
    return _listener.listen(ConnectionManager::addConnection, this);
}

void ListeningConnectionManager::ListenerThread::main()
{
    /* Should never return */
    int errorCode = this->parent.listen();

    throw std::runtime_error("Failed to listen on stratum listener socket: Error " + std::to_string(errorCode));
}
