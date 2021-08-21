#ifndef ROGUES_SAVE_H
#define ROGUES_SAVE_H

#include <stdio.h>
#include "game.h"

#define GAME_SAVE_PATH "./data/saves/eac.sav"

int save_game(const Game * game);

int load_game(Game * game);

#endif
