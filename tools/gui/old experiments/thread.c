#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

int global_run = 1;

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

void *main_keys( void )
{
    char c;
    int i = 0;
    int j = 0;

    while (1) {
        while (!kbhit()) {
            
            // usleep(10000);

            printf("\033[HKeys Thread: %d", ++i);
            // draw_box(&box);
        }

        c = getch();

        if (c == 27) {
            break;
        } else {
            printf("\033[3;%dH%c\n\n", ++j, c);
        }
        // (void)getch(); // consume the character
    }
}


int get_pos(int *y, int *x) {
    char buf[30]={0};
    int ret, i, pow;
    char ch;

    *y = 0; *x = 0;

    printf("\033[6n");
    fflush(stdout);

    for( i = 0, ch = 0; ch != 'R'; i++ )
    {
        ret = read(0, &ch, 1);
        if ( !ret ) {
        fprintf(stderr, "getpos: error reading response!\n");
        return 1;
        }
        buf[i] = ch;
        // printf("buf[%d]: \t%c \t%d\n", i, ch, ch);
    }

    

    if (i < 2) {
        printf("i < 2\n");
        return(1);
    }

    for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
        *x = *x + ( buf[i] - '0' ) * pow;

    for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
        *y = *y + ( buf[i] - '0' ) * pow;

    return 0;
}

void get_term_size(int* width, int* height) {
    // Set cur pos to really high vals.
    printf("\033[999999999;999999999H");
    get_pos(height, width);
}

void *draw_thread( void )
{
    int width;
    int height;

    while (global_run) {
        get_term_size(&width, &height);
        printf("\033[Hw: %d", width);
        // fflush(stdout);
    }
}


int main()
{
    pthread_t thread1, thread2;
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";
    int  iret1, iret2;

    // Set terminal cool mode...
    set_conio_terminal_mode();

    /* Create independent threads each of which will execute function */
    iret1 = pthread_create( &thread1, NULL, main_keys, NULL);
    iret2 = pthread_create( &thread2, NULL, draw_thread, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    pthread_join( thread1, NULL);
    global_run = 0;
    pthread_join( thread2, NULL);

    reset_terminal_mode();
    printf("Thread 1 returns: %d\n", iret1);
    printf("Thread 2 returns: %d\n", iret2);

    printf("\nDone.\n");

    exit(0);
    return 0;
}
