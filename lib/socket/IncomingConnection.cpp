#include "IncomingConnection.h"
#include "ConnectionManager.h"

#include <util/logger.h>


IncomingConnection::IncomingConnection(ConnectionManager &manager, SocketBase &&socket) : AsynchronousSocket(std::move(socket)),
    _connections(manager.connectionsHolder),
    _dangling(false)
{
    if (this->receiver.state() == ReceiverThread::FailedToStart)
    {
        mlog(WARNING, "Failed to initialize receiver, set connection to dangling");
        _dangling = true;
    }
}

void IncomingConnection::moveTo(ConnectionManager &manager)
{
    if (_connections.get() == &manager.connections())
        return;

    IncomingConnectionRef holder = _connections->remove(*this, true);

    if (!holder)
        throw std::runtime_error("Cannot take connection from manager");

    _connections = manager.connectionsHolder;
    _connections->add(std::move(holder));
}

void IncomingConnection::cleanup()
{
    mlog(DEBUG, "connection [%p]: Cleaning up", this);
    this->_connections->remove(*this);
}
