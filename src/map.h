#ifndef ROGUES_MAP_H
#define ROGUES_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "drawing.h"

#define MAP_WIDTH  220
#define MAP_HEIGHT 100
#define PIXEL_WIDTH 20

/*
The map.ansi file is assumed to be an ansi file of rectangle proportions
where each char (with ansi code) takes up PIXEL_WIDTH bytes. These bytes 
consist of the ESC char and the color settings in ansi true colour...

    xxxxxx
    xxxxxx <- IMPORTANT: ALL LINES MUST BE THE SAME LENGTH!!!!
    xxxxxx
*/

// Each line we read is 'width' pixels and there is an '\n\0' at the end.
#define READ_LINE_SIZE ((MAP_WIDTH * PIXEL_WIDTH) + 2) // <-- Very important that 
                                                       // this is right to prevent segfaults :/

#define Map Draw_Buffer

int Map_Load(Map *map, const char *filepath);

#endif
