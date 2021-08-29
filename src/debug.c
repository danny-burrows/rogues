#include "debug.h"

int d_printf(int debug_level, const char * format, ...) {

    switch (debug_level) {
        case 0:
            printf("\033[38;2;220;220;20m[DEBUG] ");
            break;
        case 1:
            printf("\033[38;2;255;107;20m[WARN] ");
            break;
        case 2:
            fprintf(stderr, "\033[1m\033[38;2;245;40;50m[ERR] ");
            break;
    }

    int r;
    va_list argptr;
    va_start(argptr, format);

    if (debug_level == 2) {
        r = vfprintf(stderr, format, argptr);
        fflush(stderr);
    } else {
        r = vprintf(format, argptr);
        fflush(stdout);
    }

    va_end(argptr);

    //Reset terminal colors.
    printf("\033[0m");
    
    return r;
}
