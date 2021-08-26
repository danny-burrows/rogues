#ifndef ROGUES_GAME_H
#define ROGUES_GAME_H

#include "ui.h"
#include "map.h"
#include "camera.h"
#include "player.h"

// Thinking about more structered data... perhaps an array for all the UI elements...
// Maybe a deeper struct for all the Map-Relative elements e.g. player, buildings e.t.c.

typedef struct {
    char version[64];
    int running;
    int loaded;
    UI_BOX box; // To be deprecated!
    UI_BOX view_port;
    UI_BOX control_surface;
    Player player;
    Camera camera;
    MAP map;
} Game;

#endif
