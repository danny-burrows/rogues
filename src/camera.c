#include "camera.h"

void camera_center_on_point(int x, int y, Camera * camera, const MAP * map) {
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