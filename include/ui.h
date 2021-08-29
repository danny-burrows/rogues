#ifndef ROGUES_UI_H
#define ROGUES_UI_H

#include <stdio.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;

    int px;
    int py;
    int pw;
    int ph;
} Ui_Box;

void draw_ui_box(const Ui_Box * box);

int print_in_box(int x, int y, const char * str, const Ui_Box * box);

void clear_ui_box(const Ui_Box * box);

void draw_ui_box_if_updated(Ui_Box * box);

void draw_title_bar(int width, const char * game_version);

void draw_health_bar(float health, Ui_Box * control_surface);

#endif
