#include "Thread.h"

#define MODULE  threading
#include "logger.h"

#include <stdexcept>



Thread::Thread() :
    _state(FailedToStart)
{
    int result;

    _thread_run_lock.aquireUnmanaged();

    result = pthread_create(&_thread, NULL, entrypoint, this);

    if (result)
    {
        this->initializationCallback(result);
        _thread_run_lock.releaseUnmanaged();
    }
    else
    {
        this->initializationCallback();
        _state = WaitingToStart;

        pthread_detach(_thread);
    }
}

Thread::~Thread()
{
    if (this->state() != Terminating)
    {
        mlog(INFO, "Force-terminating thread");
        pthread_cancel(_thread);

        if (this->state() == WaitingToStart)
            this->start();
    }
}

void Thread::start()
{
    if (this->state() == WaitingToStart)
    {
        _state = Running;
        _thread_run_lock.releaseUnmanaged();
    }
}

void Thread::initializationCallback(int errorCode)
{
    if (errorCode)
        throw std::runtime_error("Failed to initialize critical thread");
}

void Thread::entrypoint()
{
    int dummy;

    OBTAIN_LOCK(_thread_run_lock);

    this->main();

    _state = State::Terminating;

    /* Don't allow thread cancellation mid-cleanup */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &dummy);
    this->cleanupCallback();
}

void *Thread::entrypoint(void *context)
{
    int dummy;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &dummy);

    ((Thread *) context)->entrypoint();
    return NULL;
}
