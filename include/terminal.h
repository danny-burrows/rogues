#ifndef ROGUES_TERMINAL_H
#define ROGUES_TERMINAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

//
// Keyboard input stuff
//
struct termios orig_termios;

void reset_terminal_mode(void);
void set_conio_terminal_mode(void);
int kbhit(void);
int getch(void);

//
// Terminal Size
//
void get_term_size(int *restrict width, int *restrict height);

//
// Terminal Utils
//
void clear_term(void);

#endif
