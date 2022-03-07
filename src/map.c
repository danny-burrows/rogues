#include "map.h"

int Map_Load(Map *map, const char *filepath)
{    
    map->width = MAP_WIDTH;
    map->height = MAP_HEIGHT;

    FILE *file_ptr = fopen(filepath, "r");

    if (!file_ptr) {
#ifdef DCONFIGSET
        perror("[ERR] Error loading map");
#endif
        return -1;
    }

    Pixel *pxl_ptr = (Pixel *)map->data;
    char line[READ_LINE_SIZE];

    // Read map file in one line at a time.
    while (fgets(line, READ_LINE_SIZE, file_ptr)) {
        char *line_ptr = line;

        // Walk through each line attempting to populate pixel data.
        while (*line_ptr != '\n') {
            if (!sscanf(line_ptr, "\033[38;2;%u;%u;%um%c", &pxl_ptr->r, &pxl_ptr->b, &pxl_ptr->g, &pxl_ptr->ch)) return -1;

            // Interpolate background color.
            pxl_ptr->background = 1;
            pxl_ptr->bg_r  = pxl_ptr->r / 3;
            pxl_ptr->bg_g  = pxl_ptr->g / 3;
            pxl_ptr->bg_b  = pxl_ptr->b / 3;

            line_ptr += 20; pxl_ptr++;
        }
    }
    fclose(file_ptr);
    
    d_printf(INFO, "Map loaded successfully!\n");
    return 0;
}
