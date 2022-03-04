#include "map.h"

int set_map_size(Map *map) {
    // Probably need to make all this look less horrible...

    map->width = strchr(map->data, '\n') - map->data;
    map->height = (strchr(map->data, '\0') - map->data) / map->width;

    map->width /= 12;

    d_printf(INFO, "MAP WIDTH: %d\n", map->width);
    d_printf(INFO, "MAP HEIGHT: %d\n", map->height);
    return 0;
}

int load_map(Map *map) {
    int map_size;
    FILE *read_ptr;

    read_ptr = fopen(MAP_FILE_PATH, "r");

    if (read_ptr == NULL) {
#ifdef DCONFIGSET
        perror("[ERR] Error loading map");
#endif
        return -1;
    }

    fseek(read_ptr, 0, SEEK_END); // Seek to end of file
    map->size = ftell(read_ptr); // Get current file pointer
    fseek(read_ptr, 0, SEEK_SET); // Seek back to beginning of file

    if (map->size > MAX_MAP_SIZE) {
        d_printf(ERR, "Error reading map: Map file too large!\n");
        return -1;
    }

    int successfully_read = fread(map->data, map->size, 1, read_ptr);
    fclose(read_ptr);
    if (successfully_read != 1) return -1;

    set_map_size(map);

    d_printf(INFO, "Map loaded successfully!\n");
    return 0;
}
