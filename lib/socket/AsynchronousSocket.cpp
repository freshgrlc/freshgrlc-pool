#include "AsynchronousSocket.h"

#include <util/logger.h>


void AsynchronousSocket::ReceiverThread::main()
{
    this->parent.receive();
}

void AsynchronousSocket::ReceiverThread::initializationCallback(int error)
{
    if (!error)
        mlog(DEBUG, "Spawned thread %p for connection %p", this->threadId(), &this->parent);
    else
        mlog(WARNING, "Failed to spawn connection thread (error %d), this will result in a dangling Connection object", error);
}

void AsynchronousSocket::ReceiverThread::cleanupCallback()
{
    this->parent.cleanup();
}
