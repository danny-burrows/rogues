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
} UI_BOX;

void draw_ui_box(const UI_BOX * box);

void clear_ui_box(const UI_BOX * box);

void draw_ui_box_if_updated(UI_BOX * box);

void draw_title_bar(int width, const char * game_version);

void draw_health_bar(float health, UI_BOX * control_surface);

#endif
