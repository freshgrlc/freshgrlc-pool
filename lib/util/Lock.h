#ifndef UTIL_LOCK_H
#define UTIL_LOCK_H

#include <pthread.h>

class Lock
{
    public:
        class Ref
        {
            public:
                Ref(Lock &parent);
                virtual ~Ref(void);

                inline void __dummy(void) const {}

            private:
                Lock &_parent;
        };

        Lock(void);
        Lock(const Lock &) = delete;
        Lock(Lock &&) = delete;
        Lock& operator=(const Lock &) = delete;
        Lock& operator=(Lock &&) = delete;
        virtual ~Lock(void);

        Ref aquire(void);

        void aquireUnmanaged(void);
        void releaseUnmanaged(void);

    private:
        pthread_mutex_t _mutex;
};


#define OBTAIN_LOCK(lock)   const Lock::Ref &__ ## lock ## _ref = lock.aquire(); __ ## lock ## _ref.__dummy()

#endif
