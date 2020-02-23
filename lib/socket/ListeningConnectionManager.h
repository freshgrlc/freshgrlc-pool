#ifndef SOCKET_LISTENINGCONNECTIONMANAGER_H
#define SOCKET_LISTENINGCONNECTIONMANAGER_H

#include "ConnectionManager.h"

class ListeningConnectionManager : public ConnectionManager
{
    public:
        inline ListeningConnectionManager(Listener &listener) : ConnectionManager(),
            _listener(listener)
        {}

        int listen(void);

        inline Listener &listener(void)                                         { return _listener; }

    private:
        Listener &_listener;
};

#endif
