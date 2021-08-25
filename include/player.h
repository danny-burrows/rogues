#ifndef ROGUES_PLAYER_H
#define ROGUES_PLAYER_H

#include "camera.h"
#include "ui.h"
#include "map.h"

typedef struct {
    int x;
    int y;
    float health;
} Player;

int draw_player(const Player * player, const Camera * camera, const UI_BOX * view_port);

void center_camera_on_player(Player * player, Camera * camera, const UI_BOX * view_port, const MAP * map);

#endif