#include "saved_games.h"

int save_game(const Game * game) {
    FILE *write_ptr;
    write_ptr = fopen(GAME_SAVE_PATH, "wb");

    if (write_ptr == NULL) {
        perror("[ERR] Error saving game");
        return -1;
    }

    int successfully_writen = fwrite(game, sizeof(Game), 1, write_ptr);
    fclose(write_ptr);
    if (successfully_writen != 1) return -1;

    printf("[DEBUG] Saved game successfully!\n");
    return 0;
}

int load_game(Game * game) {
    FILE *read_ptr;
    read_ptr = fopen(GAME_SAVE_PATH, "rb");

    if (read_ptr == NULL) {
        perror("[ERR] Error loading game save");
        return -1;
    }

    // Use file size to attempt to check if game save is corrupt.
    // NOTE: May need a more rigorous way of checking this!
    fseek(read_ptr, 0, SEEK_END);
    long int file_size = ftell(read_ptr);
    fseek(read_ptr, 0, SEEK_SET);
    if (sizeof(*game) != file_size) {
        fprintf(stderr, "[ERR] Error reading game save (file_size check); save file might be corrupted!\n");
        return -1;
    }

    // Copy game version from default save struct.
    char version_check[64];
    strcpy(version_check, game->version);

    int successfully_read = fread(game, sizeof(Game), 1, read_ptr);
    fclose(read_ptr);
    if (successfully_read != 1) return -1;

    // Compare game save version with current game version.
    if (strcmp(version_check, game->version)) {
        fprintf(stderr, "[ERR] Game save version (%s) doesn't match current game version (%s)!\n", game->version, version_check);
        return -1;
    }

    // For debug...
    game->loaded = 1;
    printf("[DEBUG] Game save loaded successfully!\n");
    return 0;
}
