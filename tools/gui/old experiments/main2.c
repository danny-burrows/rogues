#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Box;

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

void draw_box(Box* box) {
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

    fflush(stdout);
}


int main(void) {

    Box box = {0,0,0,0};

    Box button1 = {0,0,40,5};
    Box button2 = {0,0,40,5};

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

            // box.height = height / 4;
            // box.width = width / 4;
            // box.x = (width / 2) - (box.width / 2);
            // box.y = (height / 2) - (box.height / 2);

            box.x = 4;
            box.y = 2;
            box.width = width - 7;
            box.height = height - 10;

            button1.x = 4;
            button1.y = height - 2 - button1.height;

            button2.x = width - 3 - button2.width;
            button2.y = height - 2 - button2.height;
            
            clear_no_flush();

            // Set color green.
            printf("\033[38;5;2m");

            draw_box(&box);
            draw_box(&button1);
            draw_box(&button2);
            fflush(stdout);
        }
    }

    printf("\nThank you.\n");
    return 0;
}