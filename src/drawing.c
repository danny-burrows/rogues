#include "drawing.h"

void Draw_Buffer_Copy(Draw_Buffer *dest, Draw_Buffer *source, int source_start_x, int source_start_y)
{
    for (int y = 0; y < dest->height; y++) {
        for (int x = 0; x < dest->width; x++) {

            Pixel *src_pxl  = &source->data[source_start_y + y][source_start_x + x];
            Pixel *dest_pxl = &dest->data[y][x];

            dest_pxl->ch = src_pxl->ch;
            dest_pxl->r  = src_pxl->r;
            dest_pxl->g  = src_pxl->g;
            dest_pxl->b  = src_pxl->b;

            // Background
            if (src_pxl->background) {
                dest_pxl->background = true;
                dest_pxl->bg_r  = dest_pxl->r / 3;
                dest_pxl->bg_g  = dest_pxl->g / 3;
                dest_pxl->bg_b  = dest_pxl->b / 3;
            }
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

        if (tx > buffer->width) return -1;

        buffer->data[ty][tx].ch = *string;

        // Just using white for color...
        buffer->data[ty][tx].r = 255;
        buffer->data[ty][tx].g = 255;
        buffer->data[ty][tx].b = 255;

        tx++; string++;
    }
    return 0;
}

void Draw_Buffer_Render(Draw_Buffer *buffer, int draw_start_x, int draw_start_y)
{
    Pixel *pxl;

    bool color_carryover = false;
    unsigned int r, g, b, br, bg, bb = {0};

    for (int i = 0; i < buffer->height; i++)
    {
        printf("\033[%d;%dH", draw_start_y + i, draw_start_x);

        for (int j = 0; j < buffer->width; j++)
        {
            pxl = &buffer->data[i][j];

            if (r != pxl->r || g != pxl->g || b != pxl->b || br != pxl->bg_r || bg != pxl->bg_g || bb != pxl->bg_b) {
                r = pxl->r;
                g = pxl->g;
                b = pxl->b;
                br = pxl->bg_r;
                bg = pxl->bg_g;
                bb = pxl->bg_b;

                color_carryover = false;
            }

            if (color_carryover) {
                printf("%c", pxl->ch);
            } else if (pxl->background) {
                printf("\033[38;2;%hhu;%hhu;%hhum\033[48;2;%hhu;%hhu;%hhum%c",
                    pxl->r,
                    pxl->b,
                    pxl->g,
                    pxl->bg_r,
                    pxl->bg_b,
                    pxl->bg_g,
                    pxl->ch
                );
            } else {
                printf("\033[38;2;%hhu;%hhu;%hhum%c",
                    pxl->r,
                    pxl->b,
                    pxl->g,
                    pxl->ch
                );
            }

            color_carryover = true;
        }
    }
    printf("\033[0m");
    fflush(stdout);
}