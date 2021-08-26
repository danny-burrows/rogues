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

int player_set_position(int x, int y, Player * player, const MAP * map) {
    // Check player is out of bounds of the map.
    if (x < 0 || x > map->width) {
        return -1;
    } else if (y < 0 || y > map->height) {
        return -1;
    }

    player->x = x;
    player->y = y;
}

void player_draw(const Player * player, const Camera * camera, const UI_BOX * view_port) {
    // Calc where the player is relative to the viewport.
    int screen_x = view_port->x + (player->x - camera->x);
    int screen_y = view_port->y + (player->y - camera->y);

    print_in_box(screen_x, screen_y,     " o ",  view_port);
    print_in_box(screen_x, screen_y + 1, "(|~",  view_port);
    print_in_box(screen_x, screen_y + 2, "/ \\", view_port);
}

void center_camera_on_player(Player * player, Camera * camera, const MAP * map) {
    camera->x = player->x - (camera->vw / 2);
    camera->y = player->y - (camera->vh / 2);

    if (camera->x < 0) {
        camera->x = 0;
    } else if (camera->x > (map->width - camera->vw)) {
        camera->x = map->width - camera->vw;
    }

    if (camera->y < 0) {
        camera->y = 0;
    } else if (camera->y > (map->height - camera->vh)) {
        camera->y = map->height - camera->vh;
    }
}

int player_teleport(int x, int y, Player * player, Camera * camera, MAP * map, UI_BOX * view_port) {
    player_set_position(x, y, player, map);
    center_camera_on_player(player, camera, map);
}