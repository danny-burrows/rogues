#ifndef ROGUES_CAMERA_H
#define ROGUES_CAMERA_H

typedef struct {
    int x;
    int y;
    int vw;
    int vh;
} Camera;

void camera_center_on_point(Camera * camera, int x, int y, int map_width, int map_height);

void camera_step_right(Camera * camera, int map_width);

void camera_step_left(Camera * camera);

void camera_step_up(Camera * camera);

void camera_step_down(Camera * camera, int map_height);

#endif
