#include "ConnectionHolder.h"

#include "ConnectionManager.h"
#include "NodeConnection.h"


ConnectionHolder::ConnectionHolder(ConnectionManager &parent, std::string host, uint16_t port) :
    _parent(parent),
    _host(host),
    _port(port)
{
}

void ConnectionHolder::connect()
{
    this->connection = std::make_unique<NodeConnection>(_host, _port, _parent.networkMagic(), _parent);
}

bool ConnectionHolder::connected() const
{
    return this->connection && this->connection->connected();
}
