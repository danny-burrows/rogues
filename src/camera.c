#include "camera.h"

void camera_center_on_point(Camera * camera, int x, int y, int map_width, int map_height) {
    camera->x = x - (camera->vw / 2);
    camera->y = y - (camera->vh / 2);

    // If viewport is small we stop it going off the edge.
    if (camera->x > (map_width - camera->vw)) {
        camera->x = map_width - camera->vw;
    }
    if (camera->y > (map_height - camera->vh)) {
        camera->y = map_height - camera->vh;
    }

    // Also prevent going off the small edges.
    if (camera->x < 0) {
        camera->x = 0;
    } 
    if (camera->y < 0) {
        camera->y = 0;
    }
}

void camera_step_right(Camera * camera, int map_width) {
    if (camera->x + camera->vw + 1> map_width) return;
    camera->x++;
}

void camera_step_left(Camera * camera) {
    if (camera->x - 1 < 0) return;
    camera->x--;    
}

void camera_step_up(Camera * camera) {
    if (camera->y - 1 < 0) return;
    camera->y--;
}

void camera_step_down(Camera * camera, int map_height) {
    if (camera->y + camera->vh + 1> map_height) return;
    camera->y++;
}
