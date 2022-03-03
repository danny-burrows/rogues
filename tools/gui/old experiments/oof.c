#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#include <sys/ioctl.h>

void clear_no_flush(void) {
    printf("\x1b[2J");
}

void clear(void) {
    printf("\x1b[2J");
    fflush(stdout);
}

void get_term_size(int* width, int* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *height = (int)w.ws_row;
    *width = (int)w.ws_col;
}

int main(void) {
    int pw = 0;
    int ph = 0;

    int width;
    int height;
    
    clear();
    while (1) {
        get_term_size(&width, &height);

        if (width != pw || height != ph) {
            pw = width;
            ph = height;

            printf("\x1b[2J\033[%d;%dH", height/2, width/2 - 9);
            printf("width:%d, height:%d", width, height);
            fflush(stdout);
        }
    }

    fflush(stdout);
    return 0;
} 