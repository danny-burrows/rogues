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

void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();


//
// Terminal Size
//
void get_term_size(int *restrict width, int *restrict height);

#endif
