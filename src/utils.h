#ifndef ROGUES_UTILS_H
#define ROGUES_UTILS_H

#include <stdbool.h>

typedef struct {
    unsigned int r;
    unsigned int g;
    unsigned int b;
    
    bool background;
    unsigned int bg_r;
    unsigned int bg_g;
    unsigned int bg_b;
    
    char  ch;
} Pixel;

#endif
