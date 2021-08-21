#ifndef TERMINAL_HELPER
#define TERMINAL_HELPER

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

//
// Keyboard input stuff
//
struct termios orig_termios;

void reset_terminal_mode(){
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode(){
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit(){
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch(){
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}


//
// Terminal Size
//
void get_term_size(int *restrict width, int *restrict height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *height = (int)w.ws_row;
    *width = (int)w.ws_col;
}

#endif
