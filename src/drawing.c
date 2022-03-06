#include "drawing.h"

void Draw_Buffer_Copy(Draw_Buffer *dest, Draw_Buffer *source, int source_start_x, int source_start_y) 
{
    for (int y = 0; y < dest->height; y++) {
        for (int x = 0; x < dest->width; x++) {
            dest->data[source_start_y + y][source_start_x + x].ch = source->data[source_start_y + y][source_start_x + x].ch;
            dest->data[source_start_y + y][source_start_x + x].r  = source->data[source_start_y + y][source_start_x + x].r;
            dest->data[source_start_y + y][source_start_x + x].g  = source->data[source_start_y + y][source_start_x + x].g;
            dest->data[source_start_y + y][source_start_x + x].b  = source->data[source_start_y + y][source_start_x + x].b;
        }
    }
}

int Draw_Buffer_AddString(Draw_Buffer *buffer, const char *string, int x, int y)
{
    if (x < 0 || y < 0) return -1;

    int tx = x;
    int ty = y;
    while (*string) {

        // If we see a \n char we add a newline.
        if (*string == '\n') {
            if (ty >= buffer->height) return -1;
            tx = x;
            ty++; string++;
            continue;
        }

        if (tx >= buffer->width - 1) return -1;

        buffer->data[ty][tx].ch = *string;

        // Just using while for color...
        buffer->data[ty][tx].r = 255;
        buffer->data[ty][tx].g = 255;
        buffer->data[ty][tx].b = 255;

        tx++; string++;
    }

    return 0;
}

void Draw_Buffer_Render(Draw_Buffer *buffer, int draw_start_x, int draw_start_y) 
{
    for (int i = 0; i < buffer->height; i++) {

        for (int j = 0; j < buffer->width; j++) {
            
            printf("\033[%d;%dH\033[38;2;%hhu;%hhu;%hhum%c", draw_start_y + i, draw_start_x + j, buffer->data[i][j].r, buffer->data[i][j].b, buffer->data[i][j].g, buffer->data[i][j].ch);
        
        }

    }

    printf("\033[0m");
    fflush(stdout);
}