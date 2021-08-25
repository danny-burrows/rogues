#include "player.h"

// int teleport_player(int x, int y, Player * player, const MAP * map) {
//     if (x < 0 || x > map->width) {
//         return -1;
//     } else if (y < 0 || y > map->height) {
//         return -1;
//     }

//     player->x = x;
//     player->y = y;
// }

int draw_player(const Player * player, const Camera * camera, const UI_BOX * view_port) {
    // Draw the player to the screen.
    int screen_x = view_port->x + (player->x - camera->x);
    int screen_y = view_port->y + (player->y - camera->y);

    // Check if out of bounds (not currently in view).
    if (
        screen_x < view_port->x + 1
     || screen_x > (view_port->x + view_port->width) 
     || screen_y < view_port->y + 1
     || screen_y > (view_port->y + view_port->height) 
    ) {
        return -1;
    }

    printf("\033[%d;%dH o ", screen_y, screen_x);
    printf("\033[%d;%dH(|~", screen_y + 1, screen_x);
    printf("\033[%d;%dH/ \\", screen_y + 2, screen_x);
    return 0;
}

void center_camera_on_player(Player * player, Camera * camera, const UI_BOX * view_port, const MAP * map) {
    camera->x = player->x - (view_port->width / 2);
    camera->y = player->y - (view_port->height / 2);

    if (camera->x < 0) {
        camera->x = 0;
    } else if (camera->x > (map->width - view_port->width)) {
        camera->x = map->width - view_port->width;
    }

    if (camera->y < 0) {
        camera->y = 0;
    } else if (camera->y > (map->height - view_port->height)) {
        camera->y = map->height - view_port->height;
    }
}