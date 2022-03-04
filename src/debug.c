#include "debug.h"

int d_printf(DEBUG_LEVEL debug_level, const char * format, ...) {
    
    int r = 0;

#ifdef DCONFIGSET

    switch (debug_level) {
        case INFO:
            printf("\033[38;2;220;220;20m[INFO] ");
            break;
        case WARN:
            printf("\033[38;2;255;107;20m[WARN] ");
            break;
        case ERR:
            fprintf(stderr, "\033[1m\033[38;2;245;40;50m[ERR] ");
            break;
    }

    va_list argptr;
    va_start(argptr, format);

    if (debug_level == ERR) {
        r = vfprintf(stderr, format, argptr);
        fflush(stderr);
    } else {
        r = vprintf(format, argptr);
        fflush(stdout);
    }

    va_end(argptr);

    //Reset terminal colors.
    printf("\033[0m");

#endif

    return r;
}
