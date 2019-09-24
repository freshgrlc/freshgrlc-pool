#include "logger.h"
#include "Lock.h"

#include <ctime>
#include <stdarg.h>


Lock __logger_lock;


#ifdef NDEBUG
enum LogLevel __loglevel = INFO;
#else
enum LogLevel __loglevel = DEBUG;
#endif


#define MAX_TIMESTAMP_SIZE      26

static const char * getTimestamp(char *timestamp)
{
    time_t timer;
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(timestamp, MAX_TIMESTAMP_SIZE, "%Y-%m-%d %H:%M:%S", tm_info);

    return timestamp;
}

void log_raw(const char *module, const char *level_s, int level, const char *templ, ...)
{
    char timestamp[MAX_TIMESTAMP_SIZE];
    va_list varargs;

    if (level < __loglevel)
        return;

    OBTAIN_LOCK(__logger_lock);

    fprintf(stderr, "%s: ", getTimestamp(timestamp));

    if (module)
        fprintf(stderr, "<%s> ", module);

    fprintf(stderr, "%s: ", level_s);

    va_start(varargs, templ);
    vfprintf(stderr, templ, varargs);
    va_end(varargs);

    fputc('\n', stderr);
}
