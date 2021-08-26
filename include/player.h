#ifndef ROGUES_PLAYER_H
#define ROGUES_PLAYER_H

#include "ui.h"
#include "map.h"
#include "camera.h"

typedef struct {
    int x;
    int y;
    float health;
} Player;

void player_set_health(Player * player, float health);

int player_set_position(int x, int y, Player * player, const MAP * map);

int player_teleport(int x, int y, Player * player, Camera * camera, MAP * map, UI_BOX * view_port);

void player_draw(const Player * player, const Camera * camera, const UI_BOX * view_port);

void center_camera_on_player(Player * player, Camera * camera, const MAP * map);

#endif