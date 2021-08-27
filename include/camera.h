#ifndef ROGUES_CAMERA_H
#define ROGUES_CAMERA_H

#include "map.h"

typedef struct {
    int x;
    int y;
    int vw;
    int vh;
} Camera;

void camera_center_on_point(int x, int y, Camera * camera, const MAP * map);

void camera_step_right(Camera * camera, const MAP * map);

void camera_step_left(Camera * camera, const MAP * map);

void camera_step_up(Camera * camera, const MAP * map);

void camera_step_down(Camera * camera, const MAP * map);

#endif
