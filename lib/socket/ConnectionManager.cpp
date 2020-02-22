#include "ConnectionManager.h"
#include <util.h>

#include <unistd.h>
#include <ctime>


ConnectionManager::ConnectionManager(Listener& listener) :
    _listener(listener)
{
}

void ConnectionManager::addConnection(SocketBase &&socket)
{
    OBTAIN_LOCK(_clients_lock);

    _clients.push_back(this->makeConnection(std::move(socket), *this));

    _clients.end()[-1]->enableProcessing();

    mlog(DEBUG, "Added connection, new total: %ld", _clients.size());
}

void ConnectionManager::addConnection(SocketBase &&socket, void *manager)
{
    ((ConnectionManager *) manager)->addConnection(std::move(socket));
}

void ConnectionManager::remove(ConnectionManager::Connection &connection)
{
    OBTAIN_LOCK(_clients_lock);

    for (auto iter = _clients.begin(); iter != _clients.end(); iter++)
    {
        if (iter->get()->dangling())
        {
            mlog(DEBUG, "Removing old dangling connection %ld", iter - _clients.begin());
            iter = _clients.erase(iter);
        }

        if (iter->get() == &connection)
        {
            _clients.erase(iter);
            mlog(DEBUG, "Removed connection %ld", iter - _clients.begin());
            return;
        }
    }
}

int ConnectionManager::listen()
{
    int result;

    if (!_listener.connected())
    {
        if ((result = _listener.open()))
            return result;
    }

    return _listener.listen(ConnectionManager::addConnection, this);
}

ConnectionManager::Connection::Connection(ConnectionManager &manager, SocketBase &&socket) : Socket(std::move(socket)),
    receiver(this),
    _manager(manager),
    _dangling(false)
{
    if (this->receiver.state() == ReceiverThread::FailedToStart)
    {
        mlog(WARNING, "Failed to initialize receiver, set connection to dangling");
        _dangling = true;
    }
}

void ConnectionManager::Connection::cleanup()
{
    mlog(DEBUG, "connection [%p]: Cleaning up", this);
    this->_manager.remove(*this);
}

void ConnectionManager::Connection::ReceiverThread::main()
{
    this->parent.receive();
}

void ConnectionManager::Connection::ReceiverThread::initializationCallback(int error)
{
    if (!error)
        mlog(DEBUG, "Spawned thread %p for connection %p", this->threadId(), &this->parent);
    else
        mlog(WARNING, "Failed to spawn connection thread (error %d), this will result in a dangling Connection object", error);
}

void ConnectionManager::Connection::ReceiverThread::cleanupCallback()
{
    this->parent.cleanup();
}
