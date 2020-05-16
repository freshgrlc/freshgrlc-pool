#ifndef CONNECTIONHOLDER_H
#define CONNECTIONHOLDER_H

#include <memory>

#include "NodeConnection.h"

class ConnectionManager;

class ConnectionHolder
{
    public:
        ConnectionHolder(ConnectionManager &parent, std::string host, uint16_t port);

        void connect(void);
        bool connected(void) const;

        inline std::string host(void) const     { return _host; }
        inline uint16_t port(void) const        { return _port; }

    private:
        ConnectionManager &_parent;
        std::string _host;
        uint16_t _port;

        std::unique_ptr<NodeConnection> connection;
};

#endif
