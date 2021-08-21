#ifndef MAP_HELPER
#define MAP_HELPER

#include <stdio.h>
#include <string.h>

#define MAX_MAP_SIZE 256 * 1024 // Max map size currently 256KB
#define MAP_FILE_PATH "./data/map/map.ans"

/*
The map.ans file is assumed to be an ansi file of rectangle proportions
where each char (with ansi code) takes up 12 bytes. The 12 bytes 
is for the ESC char and the color settings...

    xxxxxx
    xxxxxx <- IMPORTANT: ALL LINES MUST BE THE SAME LENGTH!!!!
    xxxxxx

We will get the dimensions by querying the number of lines
and the length of the first line.
*/

typedef struct  {
    int x;
    int y;
    int size;
    int width;
    int height;
    char data[MAX_MAP_SIZE];
} MAP;

int set_map_size(MAP *map);

int load_map(MAP *map);

#endif
