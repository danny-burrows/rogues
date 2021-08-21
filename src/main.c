#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "terminal.h"
#include "map.h"
#include "saved_games.h"
#include "game.h"

enum Direction {
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4
};

Game game = {
    "v0.0.1",
    0,
    0,
    {5, 6, 2, 1, 5, 6, 2, 1},
    {1, 2, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0},
    {0}
};

int MAP_OFFSET_X, MAP_OFFSET_Y = 0;

void clear(void) 
{
    // Resetting the terminal seems to avoid scrolling issues...
    // Maybe need to create seperate tty somehow eventually...
    printf("\033c");
}

void stop(void) 
{
    game.running = 0;
}

//
// Drawing stuff
//
void draw_box(const Box * box)
{
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
}

void clear_box(const Box * box) 
{
    for (int y = box->py; y < box->py + box->ph + 1; y++) {
        for (int x = box->px; x < box->px + box->pw + 1; x++ ) {
            printf("\033[%d;%dH ", y, x);
        }
    }
}

void draw_title_bar(int width) 
{
    printf("\033[1;34;47m");
    for (int i = 1; i < width + 1; i++) {
        printf("\033[1;%dH ", i);
    }
    printf("\033[1;1H  Edit And Command");
    printf("\033[0m");
    printf("\033[H");
}

void draw_box_if_updated(Box * b) 
{
    if (b->x != b->px 
     || b->y != b->py
     || b->width != b->pw
     || b->height != b->ph
    ) {
        clear_box(b);
        b->px = b->x; 
        b->py = b->y;
        b->pw = b->width;
        b->ph = b->height;
        draw_box(b);
    }
}

void draw_map_old(int x, int y, int w, int h) {
    int wid = 0;
    char * curLine = game.map.data;
    int i = 2;
    while(curLine && i < h + 1)
    {
        char * nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
        

        
        printf("\033[%d;1H%s", ++i, curLine);
        if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy    
        curLine = nextLine ? (nextLine+1) : NULL;
    }
    printf("\033[0m");
}

void draw_map(int x, int y, int w, int h) {

    char *xstart, *xend;

    int wid = 0;
    char * curLine = game.map.data;
    int i = 2;
    int line_no = 0;
    while(curLine && i < h + 1)
    {
        char * nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
        
        if (line_no > y){
            xstart = curLine + (x * 12);
            xend = curLine + (x * 12) + (w * 12) - 1;

            char r = *xend;
            *xend = '\0';
            printf("\033[%d;2H%s", ++i, xstart);
            *xend = r;
        }
        
        if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy    
        curLine = nextLine ? (nextLine+1) : NULL;
        line_no++;
    }
    printf("\033[0m");
    fflush(stdout);
}

void redraw_frame(void) 
{
    int term_w, term_h = {0};
    get_term_size(&term_w, &term_h);

    game.view_port.width = term_w - 1;
    game.view_port.height = (.75f * term_h) - 2;

    game.control_surface.width = term_w - 1;
    game.control_surface.y = (.75f * term_h) + 1;
    game.control_surface.height = .25f * term_h;

    clear();

    // TESTING DRAW IN MAP!
    draw_map(game.box.x, game.box.y, game.view_port.width, game.view_port.height);

    printf("\033[3;3H[DEBUG] Game Version %s", game.version);
    if (game.loaded) {
        printf("\033[4;3H[DEBUG] Game loaded successfully!");
    } else {
        printf("\033[4;3H[DEBUG] New game save created!");
    }
    draw_box(&game.box);

    draw_box(&game.view_port);
    draw_box(&game.control_surface);
    draw_title_bar(term_w);
    fflush(stdout);
}

void teleport_player(int x, int y) {
    MAP_OFFSET_X = x - (game.view_port.width / 2);
    MAP_OFFSET_Y = y - (game.view_port.height / 2);

    if (MAP_OFFSET_X < 0) {
        MAP_OFFSET_X = 0;
        game.box.x = game.view_port.x + x - 1;
    } else if (MAP_OFFSET_X > (game.map.width - game.view_port.width)) {
        int new_offset = MAP_OFFSET_X - (game.map.width - game.view_port.width);
        MAP_OFFSET_X = game.map.width - game.view_port.width;
        game.box.x = game.view_port.x + (game.view_port.width / 2) + new_offset;
    } else {
        game.box.x = game.view_port.x + (game.view_port.width / 2);
    }

    if (MAP_OFFSET_Y < 0) {
        MAP_OFFSET_Y = 0;
        game.box.y = game.view_port.y + y - 1;
    } else if (MAP_OFFSET_Y > (game.map.height - game.view_port.height)) {
        int new_offset = MAP_OFFSET_Y - (game.map.height - game.view_port.height);
        MAP_OFFSET_Y = game.map.height - game.view_port.height;
        game.box.y = game.view_port.y + (game.view_port.height / 2) + new_offset;
    } else {
        game.box.y = game.view_port.y + (game.view_port.height / 2);
    }

    draw_map(MAP_OFFSET_X, MAP_OFFSET_Y, game.view_port.width, game.view_port.height);
    draw_box(&game.box);
    fflush(stdout);
}

void step_player(enum Direction dir) {
    /*
    This is by far, without a doubt, the worst function I have ever writen in my life.
    I'm sorry.
    */

    int vmin_w = (game.view_port.width / 4) + game.view_port.x;
    int vmax_w = ((3 * game.view_port.width) / 4) + game.view_port.x;
    int vmin_h = (game.view_port.height / 4) + game.view_port.y;
    int vmax_h = ((3 * game.view_port.height) / 4) + game.view_port.y;

    if (dir == DOWN) {
        game.box.py = game.box.y;
        if (game.view_port.height < game.map.height) {
            if (MAP_OFFSET_Y == game.map.height - game.view_port.height) {
                game.box.y = game.box.y + 1 > (game.view_port.height - game.box.height) ? game.view_port.height - game.box.height : game.box.y + 1;
            } else {
                game.box.y = game.box.y + 1 > vmax_h ? vmax_h : game.box.y + 1;

                if (game.box.py == game.box.y) MAP_OFFSET_Y++;
            }
        } else {
            game.box.y = game.box.y + 1 > (game.view_port.height - game.box.height) ? game.view_port.height - game.box.height : game.box.y + 1;
        }
    } else if (dir == UP) {
        game.box.py = game.box.y;
        if (MAP_OFFSET_Y == 0) {
            game.box.y = game.box.y - 1 > game.view_port.y + 1 ? game.box.y - 1 : game.view_port.y + 1;
        } else {
            game.box.y = game.box.y - 1 < vmin_h ? vmin_h : game.box.y - 1;
            if (game.box.py == game.box.y) MAP_OFFSET_Y--;
        }
    } else if (dir == RIGHT) {
        game.box.px = game.box.x;
        if (game.view_port.width < game.map.width) {
            if (MAP_OFFSET_X == game.map.width - game.view_port.width) {
                game.box.x = game.box.x + 1 > (game.view_port.width - game.box.width) ? game.view_port.width - game.box.width : game.box.x + 1;
            } else {
                game.box.x = game.box.x + 1 > vmax_w ? vmax_w : game.box.x + 1;
                if (game.box.px == game.box.x) MAP_OFFSET_X++;
            }
        } else {
            game.box.x = game.box.x + 1 > (game.view_port.width - game.box.width) ? game.view_port.width - game.box.width : game.box.x + 1;
        }
    } else {
        game.box.px = game.box.x;
        if (MAP_OFFSET_X == 0) {
            game.box.x = game.box.x - 1 > game.view_port.x + 1 ? game.box.x - 1 : game.view_port.x + 1;
        } else {
            game.box.x = game.box.x - 1 < vmin_w ? vmin_w : game.box.x - 1;
            if (game.box.px == game.box.x) MAP_OFFSET_X--;
        }
    }

    draw_map(MAP_OFFSET_X, MAP_OFFSET_Y, game.view_port.width, game.view_port.height);
    draw_box(&game.box);
    fflush(stdout);
}

void process_input(const char input) 
{    
    printf("\033[6;3H[DEBUG] Pressed Key: %c", input);
    
    int min_w = (game.view_port.width / 4) + game.view_port.x;
    int max_w = ((3 * game.view_port.width) / 4) + game.view_port.x;

    int min_h = (game.view_port.height / 4) + game.view_port.y;
    int max_h = ((3 * game.view_port.height) / 4) + game.view_port.y;

    switch (input)
    {
        case 119: // W
            step_player(UP);
            return;
            game.box.y--;
            MAP_OFFSET_Y = game.box.y < min_h ? --MAP_OFFSET_Y : MAP_OFFSET_Y;
            break;
        case 115: // S
            step_player(DOWN);
            return;
            game.box.y++;
            MAP_OFFSET_Y = game.box.y > max_h ? ++MAP_OFFSET_Y : MAP_OFFSET_Y;
            break;
        case 97: // A
            step_player(LEFT);
            return;
            game.box.x--;
            MAP_OFFSET_X = game.box.x < min_w ? --MAP_OFFSET_X : MAP_OFFSET_X;
            break;
        case 100: // D
            step_player(RIGHT);
            return;
            game.box.x++;
            MAP_OFFSET_X = game.box.x > max_w ? ++MAP_OFFSET_X : MAP_OFFSET_X;
            break;
        case 105: // I Top Extreme Teleport.
            teleport_player(game.map.width / 2, 4);
            return;
            break;
        case 106: // J Left Extreme Teleport.
            teleport_player(4, game.map.height / 2);
            return;
            break;
        case 107: // K Bottom Extreme Teleport.
            teleport_player(game.map.width / 2, game.map.height - 4);
            return;
            break;
        case 108: // L Right Extreme Teleport.
            teleport_player(game.map.width - 4, game.map.height / 2);
            return;
            break;
        case 27:
            // Empty any remaining chars in stdin...
            while (kbhit()){
                (void)getch();
            }

            // Program will now exit.
            game.running = 0;
        default:
            break;
    }


    game.box.x = game.box.x > max_w ? max_w : game.box.x;
    game.box.x = game.box.x < min_w ? min_w : game.box.x;
    game.box.y = game.box.y > max_h ? max_h : game.box.y;
    game.box.y = game.box.y < min_h ? min_h : game.box.y;
    
    // redraw_frame();

    draw_map(MAP_OFFSET_X, MAP_OFFSET_Y, game.view_port.width, game.view_port.height);
    draw_box_if_updated(&game.box);
    fflush(stdout);
}


// 
// Main
//

void *draw_thread(void *vargp)
{
    int width, height, pw, ph = {0};

    while (game.running) {
        usleep(100000);
        get_term_size(&width, &height);
        printf("\033[5;3H[DEBUG] Terminal Size: %dx%d\n", width, height);
            
        if (width != pw || height != ph) {
            pw = width;
            ph = height;

            // Size has changed so redraw the frame...
            redraw_frame();
        }
    }
}

void *blocking_keys(void *vargp) 
{
    char c;
    int ret;
    while (game.running) {
        // Block and wait to read from stdin.
        c = getch();
        process_input(c);
    }
}

int main(void)
{
    pthread_t thread1, thread2;
    int  iret1, iret2;

#ifdef DCONFIGSET
    printf("[DEBUG] Welcome Developer! Game launched in debug mode!\n");
#endif

    // Load game...
    int r = load_game(&game);
    
    // If load failed and error is ENOENT (No such file or directory).
    if (r == -1 && errno == 2) 
    {
        printf("[DEBUG] Attempting to load map & create new game...\n");
        if (load_map(&game.map) == -1) exit(EXIT_FAILURE);

        int width, height;
        get_term_size(&width, &height);
        game.box.px = width / 2;
        game.box.py = height / 2;
        game.box.x = width / 2;
        game.box.y = height / 2;

        if (save_game(&game) == -1) exit(EXIT_FAILURE);
    } 
    else if (r == -1) 
    {
        exit(EXIT_FAILURE);
    }

    printf("Press enter to begin...\n");
    getch();

    game.running = 1;

    set_conio_terminal_mode();

    /* Create independent threads each of which will execute function */
    iret2 = pthread_create( &thread2, NULL, draw_thread, NULL);
    iret1 = pthread_create( &thread1, NULL, blocking_keys, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);

    // Clear screen.
    clear();
    reset_terminal_mode();

    // Save the game.
    save_game(&game);

    printf("[DEBUG] Graceful exit. Thanks! :-)\n");
    return 0;
}
