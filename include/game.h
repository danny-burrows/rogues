#ifndef MAIN_GAME_HEADER
#define MAIN_GAME_HEADER

#include "map.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;

    int px;
    int py;
    int pw;
    int ph;
} Box;

typedef struct {
    char version[64];
    int running;
    int loaded;
    Box box;
    Box view_port;
    Box control_surface;
    MAP map;
} Game;

#endif
