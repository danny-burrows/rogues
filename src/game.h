#ifndef ROGUES_GAME_H
#define ROGUES_GAME_H

#include "ui.h"
#include "map.h"
#include "utils.h"
#include "camera.h"
#include "player.h"
typedef enum _Render_Request {
    RENDER_REQUEST_NOT_REQUESTED,

    // Render request reasons
    RENDER_REQUEST_GAME_STARTUP,
    RENDER_REQUEST_DAYTIME_UPDATE,
    RENDER_REQUEST_PLAYER_INPUT,
    RENDER_REQUEST_WINDOW_RESIZE
} Render_Request;

typedef struct _Wall {
    Point begin;
    Point end;
} Wall;

// Thinking about more structered data... perhaps an array for all the UI elements...
// Maybe a deeper struct for all the Map-Relative elements e.g. player, buildings e.t.c.

typedef struct {
    char version[64];
    volatile int running;
    int loaded;
    int day;
    double time;
    Ui_Box view_port;
    Ui_Box control_surface;
    Player player;
    Camera camera;
    Map map;
    Wall walls[8];

    volatile Render_Request render_request;
} Game;

#endif
