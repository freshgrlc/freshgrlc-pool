#ifndef UTIL_THREAD_H
#define UTIL_THREAD_H

#include <pthread.h>

#include "Lock.h"


class Thread
{
    public:
        enum State
        {
            FailedToStart,
            WaitingToStart,
            Running,
            Stopped,
            Terminating
        };

        Thread(void);
        virtual ~Thread(void);

        void start(void);

        inline State state(void) const      { return _state; }
        inline void *threadId(void) const   { return (void *) _thread; }

    protected:
        virtual void main(void) = 0;

        inline virtual void initializationCallback(int errorCode = 0) {}
        inline virtual void cleanupCallback(void) {}

    private:
        pthread_t _thread;
        Lock _thread_run_lock;
        State _state;

        void entrypoint(void);
        static void *entrypoint(void *context);
};

#endif
