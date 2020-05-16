#ifndef UTIL_SYSTEMRANDOM_H
#define UTIL_SYSTEMRANDOM_H

#include "ByteString.h"
#include "Lock.h"


class SystemRandom
{
    public:
        SystemRandom(void);
        ~SystemRandom(void);

        ByteString get(size_t size);

        void fill(void *buffer, size_t size);

        template <typename T>
        inline void fill(T &object)     { this->fill(&object, sizeof(object)); }

        template<typename T, size_t N>
        inline void fill(T (&data)[N])  { this->fill(data, sizeof(data) * N); }

    private:
        Lock _lock;
        FILE *urandom;
};

#endif
