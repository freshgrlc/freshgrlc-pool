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


#define OBTAIN_FOREIGN_LOCK(lock, varname)      const Lock::Ref &__ ## varname = (lock).aquire(); __ ## varname.__dummy()
#define OBTAIN_LOCK(lock)                       OBTAIN_FOREIGN_LOCK(lock, lock ## _ref)

#endif
