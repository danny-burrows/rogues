#ifndef ROGUES_MAP_H
#define ROGUES_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "debug.h"


#define MAX_MAP_SIZE (221 * 100) // Map should be 220x100 + 1 per line for the \n
#define MAP_FILE_PATH "./data/map/map.ansi"

/*
The map.ansi file is assumed to be an ansi file of rectangle proportions
where each char (with ansi code) takes up 20 bytes. The 20 bytes 
is for the ESC char and the colour settings in ansi true colour...

    xxxxxx
    xxxxxx <- IMPORTANT: ALL LINES MUST BE THE SAME LENGTH!!!!
    xxxxxx

We will get the dimensions by querying the number of lines
and the length of the first line.
*/

typedef struct  {
    int size;
    int width;
    int height;
    
    char  data[MAX_MAP_SIZE];
    Color color_data[MAX_MAP_SIZE * sizeof(Color)];
} Map;

int set_map_size(Map *map);

int load_map(Map *map);

#endif
