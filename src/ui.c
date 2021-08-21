#include "ui.h"

void draw_ui_box(const UI_BOX * box)
{
    for (int x = box->x; x < box->x + box->width; x++ ) {
        printf("\033[%d;%dH─", box->y, x);
        printf("\033[%d;%dH─", box->y + box->height, x);
    }

    for (int y = box->y; y < box->y + box->height; y++) {
        printf("\033[%d;%dH│", y, box->x);
        printf("\033[%d;%dH│", y, box->x + box->width);
    }

    printf("\033[%d;%dH┐", box->y, box->x + box->width);
    printf("\033[%d;%dH┌", box->y, box->x);
    printf("\033[%d;%dH┘", box->y + box->height, box->x + box->width);
    printf("\033[%d;%dH└", box->y + box->height, box->x);
}

void clear_ui_box(const UI_BOX * box) 
{
    for (int y = box->py; y < box->py + box->ph + 1; y++) {
        for (int x = box->px; x < box->px + box->pw + 1; x++ ) {
            printf("\033[%d;%dH ", y, x);
        }
    }
}

void draw_ui_box_if_updated(UI_BOX * box) 
{
    if (box->x != box->px 
     || box->y != box->py
     || box->width != box->pw
     || box->height != box->ph
    ) {
        clear_ui_box(box);
        box->px = box->x; 
        box->py = box->y;
        box->pw = box->width;
        box->ph = box->height;
        draw_ui_box(box);
    }
}

void draw_title_bar(int width) 
{
    printf("\033[1;34;47m");
    for (int i = 1; i < width + 1; i++) {
        printf("\033[1;%dH ", i);
    }
    printf("\033[1;1H  Edit And Command");
    printf("\033[0m");
    printf("\033[H");
}
