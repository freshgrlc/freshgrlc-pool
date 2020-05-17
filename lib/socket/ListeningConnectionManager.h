#ifndef SOCKET_LISTENINGCONNECTIONMANAGER_H
#define SOCKET_LISTENINGCONNECTIONMANAGER_H

#include "ConnectionManager.h"

#include <util/Thread.h>


class ListeningConnectionManager : public ConnectionManager
{
    public:
        inline ListeningConnectionManager(Listener &&listener) : ConnectionManager(),
            _listener(std::move(listener)),
            _listenerThread(*this)
        {}

        inline Listener &listener(void)             { return _listener; }
        inline void startProcessing(void)           { _listenerThread.start(); }

    private:
        class ListenerThread : public Thread
        {
            public:
                inline ListenerThread(ListeningConnectionManager &parent) : parent(parent) {}
                void main(void) override;

            private:
                ListeningConnectionManager &parent;
        };

        Listener _listener;
        ListenerThread _listenerThread;

        int listen(void);
};

#endif
