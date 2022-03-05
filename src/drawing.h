#ifndef ROGUES_DRAWING_H
#define ROGUES_DRAWING_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "color.h"

#define DRAW_BUFFER_MAX_X 1000
#define DRAW_BUFFER_MAX_Y 250

typedef struct {

    int w;
    int h;

    char data[DRAW_BUFFER_MAX_Y][DRAW_BUFFER_MAX_X];
    Color colormap[DRAW_BUFFER_MAX_Y][DRAW_BUFFER_MAX_X];

} Draw_Buffer;

void Draw_Buffer_Fill(Draw_Buffer *buffer, char *source_string, int source_start_x, int source_start_y);

int  Draw_Buffer_AddString(Draw_Buffer *buffer, const char *string, int string_len, int x, int y);

void Draw_Buffer_Render(Draw_Buffer *buffer, int draw_start_x, int draw_start_y);

void Draw_Buffer_Display(Draw_Buffer *buffer, int draw_start_x, int draw_start_y);

#endif
