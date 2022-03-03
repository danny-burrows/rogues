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


void *main_keys( void *vargp )
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

void *draw_thread( int num )
{
    int i = 0;
    while (global_run) {
        printf("\033[%d;0HThread %d: %d", 4 + num, num, ++i);
    }
}


int main()
{
    pthread_t thread1, thread2, thread3, thread4, thread5, thread6, thread7, thread8;
    int  iret1, iret2;

    // Set terminal cool mode...
    set_conio_terminal_mode();

    /* Create independent threads each of which will execute function */
    iret1 = pthread_create( &thread1, NULL, main_keys, NULL);
    iret2 = pthread_create( &thread2, NULL, draw_thread, (void*) 1);
    iret2 = pthread_create( &thread3, NULL, draw_thread, (void*) 2);
    iret2 = pthread_create( &thread4, NULL, draw_thread, (void*) 3);
    iret2 = pthread_create( &thread5, NULL, draw_thread, (void*) 4);
    iret2 = pthread_create( &thread6, NULL, draw_thread, (void*) 5);
    iret2 = pthread_create( &thread7, NULL, draw_thread, (void*) 6);
    iret2 = pthread_create( &thread8, NULL, draw_thread, (void*) 7);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    pthread_join( thread1, NULL);
    global_run = 0;
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);
    pthread_join( thread4, NULL);
    pthread_join( thread5, NULL);
    pthread_join( thread6, NULL);
    pthread_join( thread7, NULL);
    pthread_join( thread8, NULL);

    reset_terminal_mode();
    printf("Thread 1 returns: %d\n", iret1);
    printf("Thread 2 returns: %d\n", iret2);

    printf("\nDone.\n");

    exit(0);
    return 0;
}
