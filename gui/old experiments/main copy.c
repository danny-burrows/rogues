#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void clear_no_flush(void) {
    printf("\x1b[2J");
}

void clear(void) {
    printf("\x1b[2J");
    fflush(stdout);
}

int get_pos(int *y, int *x) {
    char buf[30]={0};
    int ret, i, pow;
    char ch;

    *y = 0; *x = 0;

    struct termios term, restore;

    tcgetattr(0, &term);
    tcgetattr(0, &restore);
    term.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(0, TCSANOW, &term);

    printf("\033[6n");
    fflush(stdout);

    for( i = 0, ch = 0; ch != 'R'; i++ )
    {
        ret = read(0, &ch, 1);
        if ( !ret ) {
        tcsetattr(0, TCSANOW, &restore);
        fprintf(stderr, "getpos: error reading response!\n");
        return 1;
        }
        buf[i] = ch;
        // printf("buf[%d]: \t%c \t%d\n", i, ch, ch);
    }

    if (i < 2) {
        tcsetattr(0, TCSANOW, &restore);
        printf("i < 2\n");
        return(1);
    }

    for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
        *x = *x + ( buf[i] - '0' ) * pow;

    for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
        *y = *y + ( buf[i] - '0' ) * pow;

    tcsetattr(0, TCSANOW, &restore);
    return 0;
}

void get_term_size(int* width, int* height) {
    // Set cur pos to really high vals.
    printf("\033[999999999;999999999H");

    get_pos(height, width);

    // Reset curor and ourput
    // printf("\033[H");
    // printf("x:%d, y:%d\n", x, y);
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