#ifndef MAIN_GAME_HEADER
#define MAIN_GAME_HEADER

#include "ui.h"
#include "map.h"

typedef struct {
    char version[64];
    int running;
    int loaded;
    UI_BOX box;
    UI_BOX view_port;
    UI_BOX control_surface;
    MAP map;
} Game;

#endif
