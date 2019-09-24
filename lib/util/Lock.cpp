#include "Lock.h"

#include "logger.h"

#include <stdlib.h>


Lock::Lock()
{
    int result = pthread_mutex_init(&_mutex, NULL);

    if (result)
    {
        log(CRITICAL, "Unable to create mutex: Error %d", result);
        exit(1);
    }
}

Lock::~Lock()
{
    pthread_mutex_destroy(&_mutex);
}

Lock::Ref Lock::aquire()
{
    return Ref(*this);
}

void Lock::aquireUnmanaged()
{
    pthread_mutex_lock(&_mutex);
}

void Lock::releaseUnmanaged()
{
    pthread_mutex_unlock(&_mutex);
}

Lock::Ref::Ref(Lock& parent) :
    _parent(parent)
{
    _parent.aquireUnmanaged();
}

Lock::Lock::Ref::~Ref()
{
    _parent.releaseUnmanaged();
}
