#include "camera.h"

void camera_center_on_point(int x, int y, Camera * camera, const Map * map) {
    camera->x = x - (camera->vw / 2);
    camera->y = y - (camera->vh / 2);

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

void camera_step_right(Camera * camera, const Map * map) {
    if (camera->x + camera->vw + 1 >= map->width) return;
    camera->x++;
}

void camera_step_left(Camera * camera, const Map * map) {
    if (camera->x - 1 <= 0) return;
    camera->x--;    
}

void camera_step_up(Camera * camera, const Map * map) {
    if (camera->y - 1 <= 0) return;
    camera->y--;
}

void camera_step_down(Camera * camera, const Map * map) {
    if (camera->y + camera->vh + 1 >= map->height) return;
    camera->y++;
}