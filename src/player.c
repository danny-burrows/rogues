#include "player.h"

#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3

void player_set_health(Player * player, float health) {
    if (health < 0) {
        // Player death.
    }

    player->health = health;
    // draw_health_bar();
}

int player_set_position(int x, int y, Player * player, const Map * map) {
    // Check player is out of bounds of the map.
    if (x < 0 || x > map->width) {
        return -1;
    } else if (y < 0 || y > map->height) {
        return -1;
    }

    player->x = x;
    player->y = y;
}

void player_draw(const Player * player, Draw_Buffer *draw_buff, const Camera * camera, const Ui_Box * view_port) {
    int x = (player->x - camera->x);
    int y = player->y - camera->y;

    char player_texture[] = 
"\
 o ^\n\
(|~|\n\
/ \\\
";

    Draw_Buffer_AddString(draw_buff, player_texture, sizeof(player_texture), x, y);
}

int player_teleport(int x, int y, Player * player, Camera * camera, Map * map, Ui_Box * view_port) {
    player_set_position(x, y, player, map);
    camera_center_on_point(camera, player->x, player->y, map->width, map->height);
}

void player_step_right(Player * player, Camera * camera, Map * map) {

    if (player->x + 1 + 4 > map->width) return;
    //                  ^Player width

    int max_x = ((3 * camera->vw) / 4) + camera->x;

    // Check if player is stepping outside the inner area...
    if (player->x++ > max_x) {
        camera_step_right(camera, map->width); // Camera should only move if it can...
    }

}

void player_step_left(Player * player, Camera * camera, Map * map) {

    if (player->x - 1 < 0) return;

    int min_x = (camera->vw / 4) + camera->x;

    if (player->x-- < min_x) {
        camera_step_left(camera);
    }
    
}

void player_step_up(Player * player, Camera * camera, Map * map) {

    if (player->y - 1 < 0) return;

    int min_y = (camera->vh / 4) + camera->y;

    if (player->y-- < min_y) {
        camera_step_up(camera);
    }

}

void player_step_down(Player * player, Camera * camera, Map * map) {
    
    if (player->y + 1 + 5 > map->height) return;
    //                  ^Player height

    int max_y = ((3 * camera->vh) / 4) + camera->y - 3;

    if (player->y++ > max_y) {
        camera_step_down(camera, map->height);
    }

}