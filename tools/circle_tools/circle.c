#include <math.h>
#include <stdio.h>

#include "terminal.h"

#define PI 3.14159f


void circle_wipe_screen() {
    int w, h;
    get_term_size(&w, &h);

    int max_r = w > h ? w / 2 : h / 2;

    double cx = w / 2;
    double cy = h / 2 + 1;
    
    for (int i = 1; i < max_r; i++){
        usleep(13000);
        
        int res = 360;
    
        for (int j = 0; j < res; j++) {
            double a = j * (PI / 180);
            double x = cx + i * cos(a) * 2.1f; // x2 because chars are taller than they are wide!
            double y = cy + i * sin(a);

            int i_x = (int)x;
            int i_y = (int)y;

            if (0 < i_x && i_x < w - 1 && 0 < i_y && i_y < h) {
                printf("\033[%d;%dHXXX\n", i_y, (int)x);
            }
        }
    }
}


void draw_circle(int cx, int cy, int r) {
    int res = 360;
    
    for (int i = 0; i < res; i++) {
        double a = i * (PI / 180);
        double x = cx + r * cos(a) * 2.1f; // x2 because chars are taller than they are wide!
        double y = cy + r * sin(a);

        printf("\033[%d;%dHXXX\n", (int)y, (int)x);
    }
}


int main(void) {

    circle_wipe_screen();
    return 0;

    int w, h;
    get_term_size(&w, &h);

    double cx = w / 2;
    double cy = h / 2 + 1;
    
    for (int i = 1; i < (h / 2); i++){
        usleep(13000);
        draw_circle(cx, cy, i);
    }

    printf("\033[%d;%dH CIRCLE! ", h/2 + 1, w/2 - 4);
    printf("\033[H\n");
    return 0;
}
