#ifndef UTIL_LOGGER_H
#define UTIL_LOGGER_H

#include <stdio.h>

#include "Lock.h"

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

extern enum LogLevel __loglevel;
extern Lock __logger_lock;

#ifdef MODULE
#define __MODULE_LOGLEVEL(m)    __loglevel_ ## m
#define _MODULE_LOGLEVEL(m)     __MODULE_LOGLEVEL(m)
#define MODULE_LOGLEVEL         _MODULE_LOGLEVEL(MODULE)

enum LogLevel __attribute__((weak)) MODULE_LOGLEVEL = INFO;

#define _mlog(module, level, ...)   log_raw(module, #level, level, __VA_ARGS__)
#define mlog(level, ...)            do { if (MODULE_LOGLEVEL <= level) _mlog(__stringify(MODULE), level, __VA_ARGS__); } while (0)
#endif

#define ___stringify(x)             #x
#define __stringify(x)              ___stringify(x)

#define log(level, ...)             log_raw(NULL, #level, level, __VA_ARGS__)

void log_raw(const char *module, const char *level_s, int level, const char *templ, ...);


#endif
