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

void player_draw(const Player * player, const Camera * camera, const Ui_Box * view_port) {
    // Calc where the player is relative to the viewport.
    int screen_x = view_port->x + (player->x - camera->x);
    int screen_y = view_port->y + (player->y - camera->y);

    print_in_box(screen_x, screen_y,     " o ^",  view_port);
    print_in_box(screen_x, screen_y + 1, "(|~|",  view_port);
    print_in_box(screen_x, screen_y + 2, "/ \\", view_port);
}

void center_camera_on_player(Player * player, Camera * camera, const Map * map) {
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

int player_teleport(int x, int y, Player * player, Camera * camera, Map * map, Ui_Box * view_port) {
    player_set_position(x, y, player, map);
    center_camera_on_player(player, camera, map);
}

void player_step_right(Player * player, Camera * camera, Map * map) {
    int max_x = ((3 * camera->vw) / 4) + camera->x;

    // Check if player is stepping outside the inner area...
    if (player->x++ > max_x) {
        camera_step_right(camera, map); // Camera should only move if it can...
    }

}

void player_step_left(Player * player, Camera * camera, Map * map) {
    int min_x = (camera->vw / 4) + camera->x;

    if (player->x-- < min_x) {
        camera_step_left(camera, map);
    }
    
}

void player_step_up(Player * player, Camera * camera, Map * map) {
    int min_y = (camera->vh / 4) + camera->y;

    if (player->y-- < min_y) {
        camera_step_up(camera, map);
    }

}

void player_step_down(Player * player, Camera * camera, Map * map) {
    int max_y = ((3 * camera->vh) / 4) + camera->y - 3;

    if (player->y++ > max_y) {
        camera_step_down(camera, map);
    }

}