#include "terminal.h"

//
// Keyboard input stuff
//
void reset_terminal_mode(void){
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode(void){
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit(void){
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch(void){
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

//
// Terminal Utils
//
void clear_term(void)
{
    printf("\0332J");
}

void init_terminal(char * game_version) {

    // Use alternative stdout buffer. (This means users terminal is preserved for when they exit)
    printf("\033[?1049h");

    // Set title
    printf("\033]0;Rogues - %s\x07", game_version);

    // Set cursor to top of screen.
    printf("\033[1;1H");

    fflush(stdout);
}

void reset_terminal(void) {
    // Switch back to main stdout buffer.
    printf("\033[?1049l");
}