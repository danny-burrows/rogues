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

int player_set_position(int x, int y, Player * player, const Map * map);

int player_teleport(int x, int y, Player * player, Camera * camera, Map * map, Ui_Box * view_port);

void player_draw(const Player * player, const Camera * camera, const Ui_Box * view_port);

void center_camera_on_player(Player * player, Camera * camera, const Map * map);

// Player movement
void player_step_right(Player * player, Camera * camera, Map * map);
void player_step_left(Player * player, Camera * camera, Map * map);
void player_step_up(Player * player, Camera * camera, Map * map);
void player_step_down(Player * player, Camera * camera, Map * map);

#endif