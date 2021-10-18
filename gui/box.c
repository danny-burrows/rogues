#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For terminal size...
#include <unistd.h>
#include <sys/ioctl.h>

// Keyboard input...
#include <termios.h>


void clear(void) {
    // Resetting the terminal seems to avoid scrolling issues...
    printf("\033c");

    // printf("\033[2J");
    // printf("\033[H");
}


//
// Terminal size
//

void get_term_size(int* width, int* height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *height = (int)w.ws_row;
    *width = (int)w.ws_col;
}


//
// Keyboard input stuff
//

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}


//
// Box stuff
//

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Box;

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


//
// Main
//

int main(int argc, char *argv[])
{
    set_conio_terminal_mode();

    Box box = {5,5,5,5};

    int pw = 0;
    int ph = 0;

    int width;
    int height;

    char c;

    while (1) {
        while (!kbhit()) {
            get_term_size(&width, &height);
            
            if (width != pw || height != ph) {
                pw = width;
                ph = height;
                box.x = 4;
                box.y = 2;
                box.width = width - 7;
                box.height = height - 10;

                clear();
                draw_box(&box);
            }
        }

        c = getch();

        if (c == 27) {
            break;
        } else {
            printf("\033[1;1HPressed: %c\n\n", c);
        }
        // (void)getch(); // consume the character
    }

    reset_terminal_mode();
    clear();
    printf("\033[H");
    printf("Done.\n");
    return 0;
}