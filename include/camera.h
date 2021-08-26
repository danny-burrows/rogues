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

#endif
