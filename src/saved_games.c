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

    int successfully_read = fread(game, sizeof(Game), 1, read_ptr);
    fclose(read_ptr);
    if (successfully_read != 1) return -1;

    // For debug...
    game->loaded = 1;
    printf("[DEBUG] Game save loaded successfully!\n");
    return 0;
}
