#include "ReconnectingClient.h"

#include <unistd.h>

#include <util/logger.h>


ReconnectingClient::ReconnectingClient(const std::string &host, int port, bool startReceiver) : AsynchronousSocket(openConnectionTo(host, port), startReceiver),
    host(host),
    port(port)
{
    if (this->connected())
        this->onReconnect();
}

ReconnectingClient::ReconnectingClient(std::string &&host, int port, bool startReceiver) : AsynchronousSocket(openConnectionTo(host, port), startReceiver),
    host(std::move(host)),
    port(std::move(port))
{
    if (this->connected())
        this->onReconnect();
}

int ReconnectingClient::openConnection()
{
    mlog(INFO, "Attempting reconnecting to %s:%d", this->host.c_str(), this->port);
    return openConnectionTo(this->host, this->port);
}

void ReconnectingClient::cleanup()
{
    /*
     * From the terminating listening thread,
     * we reconnect and create a new thread
     * on our place, just before actual exiting.
     */

    this->receiver.setOwnState(Thread::Running);

    while (!this->connected())
    {
        sleep(1);
        _fd = openConnection();
    }

    this->onReconnect();

    this->receiver.setOwnState(Thread::Terminating);
    this->receiver.~ReceiverThread();
    new (&this->receiver) ReceiverThread(this);

    this->receiver.start();
}
