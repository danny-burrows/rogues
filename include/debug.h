#ifndef ROGUES_DEBUG_H
#define ROGUES_DEBUG_H

#include <stdio.h>
#include <stdarg.h>

enum DEBUG_LEVELS {
    DEBUG,
    WARN,
    ERR
};

int d_printf(int debug_level, const char * format, ...);

#endif