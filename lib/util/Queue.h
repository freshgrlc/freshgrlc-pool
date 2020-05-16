#ifndef UTIL_QUEUE_H
#define UTIL_QUEUE_H

#include <vector>
#include <memory>
#include <stdexcept>

#include <semaphore.h>

#include "Lock.h"


template <class T>
class Queue
{
    public:
        typedef std::unique_ptr<T> TRef;

        Queue(void)
        {
            if (sem_init(&_semaphore, 0, 0) < 0)
                throw std::runtime_error("Cannot initialize semaphore for Queue object: error " + std::to_string(errno));
        }

        virtual ~Queue(void)
        {
            sem_destroy(&_semaphore);
        }

        void queue(T &&object)
        {
            OBTAIN_LOCK(_lock);

            _queue.push_back(std::make_unique<T>(std::move(object)));
            sem_post(&_semaphore);
        }

        bool hasMessage(void)
        {
            return this->messagesWaiting() > 0;
        }

        int messagesWaiting(void)
        {
            int currentValue = 0;

            if (sem_getvalue(&_semaphore, &currentValue) < 0)
                return false;

            return currentValue;
        }

        TRef waitForMessage(void)
        {
            if (sem_wait(&_semaphore) < 0)
                throw std::runtime_error("Semaphore wait error: error " + std::to_string(errno));

            OBTAIN_LOCK(_lock);

            TRef message = std::move(_queue[0]);

            for (int i = 1; i < _queue.size(); i++)
                _queue[i-1] = std::move(_queue[i]);

            _queue.pop_back();
            return message;
        }

    private:
        sem_t _semaphore;
        Lock _lock;
        std::vector<TRef> _queue;
};

#endif
