#include "ConnectionManager.h"
#include <util.h>

#include <unistd.h>
#include <ctime>


ConnectionManager::ConnectionManager(Listener& listener) :
    _listener(listener)
{
}

void ConnectionManager::addConnection(Socket &&socket)
{
    OBTAIN_LOCK(_clients_lock);

    _clients.push_back(this->makeConnection(std::move(socket), *this));

    _clients.end()[-1]->enableProcessing();

    mlog(DEBUG, "Added connection, new total: %ld", _clients.size());
}

void ConnectionManager::addConnection(Socket &&socket, void *manager)
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

ConnectionManager::Connection::Connection(ConnectionManager &manager, Socket &&socket) : Socket(std::move(socket)),
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

void ConnectionManager::Connection::send(const Packet &packet)
{
    ssize_t len = write(_fd, packet.data, packet.length);

    if (len > 0)
    {
        if ((size_t) len < packet.length)
        {
            mlog(WARNING, "Unable to write entire packet, retrying...");
            this->send(Packet(packet.data + len, packet.length - len));
        }
        return;
    }
    else if (!len)
        mlog(DEBUG, "connection [%p]: Closed by peer", this);
    else
        mlog(ERROR, "connection [%p]: Write error %d", this, errno);
}

void ConnectionManager::Connection::receive()
{
    uint8_t buffer[8192];
    int len;

    for (;;)
    {
        len = read(_fd, buffer, sizeof(buffer));

        if (len > 0)
            this->onReceive(Packet(buffer, len));
        else
        {
            if (!len)
                mlog(DEBUG, "connection [%p]: Closed by peer", this);
            else
                mlog(ERROR, "connection [%p]: Receive error %d", this, errno);

            this->close();
            break;
        }
    }
}

ConnectionManager::Connection::ReceiverThread::ReceiverThread(Connection *parent) :
    parent(*parent),
    _state(FailedToStart)
{
    int result;

    _thread_run_lock.aquireUnmanaged();

    result = pthread_create(&_thread, NULL, entrypoint, this);

    if (result)
    {
        mlog(WARNING, "Failed to spawn connection thread (error %d), this will result in a dangling Connection object", result);
        _thread_run_lock.releaseUnmanaged();
    }
    else
    {
        mlog(DEBUG, "Spawned thread %p for connection %p", (void *) _thread, &this->parent);
        _state = WaitingToStart;

        pthread_detach(_thread);
    }
}

ConnectionManager::Connection::ReceiverThread::~ReceiverThread()
{
    if (this->state() != Terminating)
    {
        mlog(DEBUG, "Terminating receiver thread");
        pthread_cancel(_thread);

        if (this->state() == WaitingToStart)
            this->run();
    }
}

void ConnectionManager::Connection::ReceiverThread::run()
{
    if (this->state() == WaitingToStart)
    {
        _state = Running;
        _thread_run_lock.releaseUnmanaged();
    }
}

void ConnectionManager::Connection::ReceiverThread::receiver()
{
    int dummy;

    OBTAIN_LOCK(_thread_run_lock);

    this->parent.receive();

    _state = State::Terminating;

    /* Don't allow thread cancellation mid-cleanup */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &dummy);
    this->parent.cleanup();
}

void *ConnectionManager::Connection::ReceiverThread::entrypoint(void *context)
{
    int dummy;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy);

    ((ConnectionManager::Connection::ReceiverThread *) context)->receiver();
    return NULL;
}

