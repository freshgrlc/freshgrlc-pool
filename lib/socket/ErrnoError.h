#ifndef SOCKER_ERRNOERROR_H
#define SOCKER_ERRNOERROR_H

#include <errno.h>

#include <logger.h>


#define ERRNO_ERROR()   ErrnoError(__PRETTY_FUNCTION__, __LINE__)


class ErrnoError
{
    public:
        inline ErrnoError(const char *func, int line) : _errno(errno) {
            log(ERROR, "Error %d at %s:%d", _errno, func, line);
        }

        inline operator int (void) {
            return _errno;
        }

    private:
        int _errno;
};

#endif
