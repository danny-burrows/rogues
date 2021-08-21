#ifndef ROGUES_GAME_H
#define ROGUES_GAME_H

#include "ui.h"
#include "map.h"
#include "camera.h"

typedef struct {
    char version[64];
    int running;
    int loaded;
    UI_BOX box;
    UI_BOX view_port;
    UI_BOX control_surface;
    Camera camera;
    MAP map;
} Game;

#endif
