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

void draw_title_bar(int width, const char * game_version) 
{
    printf("\033[1;34;47m");
    for (int i = 1; i < width + 1; i++) {
        printf("\033[1;%dH ", i);
    }
    printf("\033[38;2;170;70;70m\033[1;1H  Rogues - %s", game_version);
    printf("\033[0m");
    printf("\033[H");
}

void draw_health_bar(float player_health, UI_BOX * control_surface) {
    int x = control_surface->x + control_surface->width - 6;
    int y = control_surface->y;

    // Draw border...
    printf("\033[%d;%dH┬", y, x);
    for (int fy = y + 1; fy < control_surface->y + control_surface->height; fy++) {
        printf("\033[%d;%dH│", fy, x);
    }
    printf("\033[%d;%dH┴", (control_surface->y + control_surface->height), x);


    int bars = (float)(control_surface->height - 2) * (player_health / 100);

    for (int fy = control_surface->y + (control_surface->height - 2 - bars); bars > 0; bars--) {
        int magic_gradient_color = (255 - bars * 20) % 255;
        magic_gradient_color = magic_gradient_color < 0 ? 0 : magic_gradient_color;

        printf("\033[%d;%dH\033[48;2;%d;255;%dm     ", fy + bars, x + 1, magic_gradient_color, magic_gradient_color);
    }

    printf("\033[%d;%dH\033[38;2;0;0;0m\033[48;2;255;255;255m%.1f", control_surface->y + control_surface->height - 1, x + 1, player_health);
}