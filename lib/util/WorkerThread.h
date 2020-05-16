#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "Thread.h"
#include "Queue.h"


template <class T>
class WorkerThread : public Thread
{
    public:
        typedef std::unique_ptr<T> EventRef;
        typedef Queue<T> EventQueue;

        WorkerThread(EventQueue &eventQueue, bool autoStart = true) : Thread(),
            eventQueue(eventQueue)
        {
            if (autoStart)
                this->start();
        }

    protected:
        virtual void processEvent(const EventRef &event) = 0;

    private:
        EventQueue &eventQueue;

        void main(void) override
        {
            for (;;)
                this->processEvent(this->eventQueue.waitForMessage());
        }
};

#endif
