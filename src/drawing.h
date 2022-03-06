#ifndef ROGUES_DRAWING_H
#define ROGUES_DRAWING_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "utils.h"

#define DRAW_BUFFER_MAX_X 220
#define DRAW_BUFFER_MAX_Y 100

typedef struct {
    int width;
    int height;

    Pixel data[DRAW_BUFFER_MAX_Y][DRAW_BUFFER_MAX_X];
} Draw_Buffer;

void Draw_Buffer_Copy(Draw_Buffer *dest, Draw_Buffer *source, int source_start_x, int source_start_y);

int  Draw_Buffer_AddString(Draw_Buffer *buffer, const char *string, int x, int y);

void Draw_Buffer_Render(Draw_Buffer *buffer, int draw_start_x, int draw_start_y);

#endif
