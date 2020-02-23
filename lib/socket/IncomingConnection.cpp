#include "IncomingConnection.h"
#include "ConnectionManager.h"

#include <util/logger.h>


IncomingConnection::IncomingConnection(ConnectionManager &manager, SocketBase &&socket) : Socket(std::move(socket)),
    receiver(this),
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

void IncomingConnection::ReceiverThread::main()
{
    this->parent.receive();
}

void IncomingConnection::ReceiverThread::initializationCallback(int error)
{
    if (!error)
        mlog(DEBUG, "Spawned thread %p for connection %p", this->threadId(), &this->parent);
    else
        mlog(WARNING, "Failed to spawn connection thread (error %d), this will result in a dangling Connection object", error);
}

void IncomingConnection::ReceiverThread::cleanupCallback()
{
    this->parent.cleanup();
}
