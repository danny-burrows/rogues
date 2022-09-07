#ifndef ROGUES_UTILS_H
#define ROGUES_UTILS_H

#include <stdlib.h>
#include <stdbool.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

typedef struct {
    unsigned int r;
    unsigned int g;
    unsigned int b;
    
    bool background;
    unsigned int bg_r;
    unsigned int bg_g;
    unsigned int bg_b;
    
    bool bold;
    char  ch;
} Pixel;

typedef struct {
    int x;
    int y;
} Point;

bool line_segments_intersect(Point p1, Point q1, Point p2, Point q2);
bool point_is_on_line_segment(Point p, Point s1, Point s2);

#endif
