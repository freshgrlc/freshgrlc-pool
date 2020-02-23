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
