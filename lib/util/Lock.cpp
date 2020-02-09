#include "Lock.h"

#include "logger.h"

#include <stdlib.h>


#ifdef LOCK_DEBUG

#define ldbg(...)           do { if (&_parent != &__logger_lock) _mlog("locking", DEBUG, __VA_ARGS__); } while (0)
extern Lock __logger_lock;

#else
#define ldbg(...)
#endif


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
    ldbg("Acquiring lock %p", &_parent);
    _parent.aquireUnmanaged();
    ldbg("Acquired  lock %p", &_parent);
}

Lock::Lock::Ref::~Ref()
{
    _parent.releaseUnmanaged();
    ldbg("Released  lock %p", &_parent);
}
