#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "game.h"

#include "ui.h"
#include "map.h"
#include "camera.h"
#include "player.h"
#include "terminal.h"
#include "saved_games.h"

enum Direction {
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4
};

Game game = {
    .version = "v0.0.5",
    .running = 0,
    .loaded = 0,
    .box = {5, 6, 2, 1, 5, 6, 2, 1},
    .view_port = {1, 2, 1, 1, 0, 0, 0, 0},
    .control_surface = {1, 1, 1, 1, 0, 0, 0, 0},
    .player = {100, 20, 100.0f},
    .camera = {0},
    .map = {0}
};

void clear_term(void) 
{
    // Resetting the terminal seems to avoid scrolling issues...
    // Maybe need to create seperate tty somehow eventually...
    printf("\033c");
}

void draw_map(UI_BOX * container) {
    char *xstart, *xend;

    int wid = 0;
    char * curLine = game.map.data;
    int i = 2;
    int line_no = 0;
    while(curLine && i < container->height + 1)
    {
        char * nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
        
        if (line_no > game.camera.y){
            xstart = curLine + (game.camera.x * 12);
            xend = curLine + (game.camera.x * 12) + (container->width * 12) - 1;

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
    game.control_surface.height = .25f * term_h - 1;

    clear_term();

    // TESTING DRAW IN MAP!
    draw_map(&game.view_port);

    printf("\033[3;3H[DEBUG] Game Version %s", game.version);
    if (game.loaded) {
        printf("\033[4;3H[DEBUG] Game loaded successfully!");
    } else {
        printf("\033[4;3H[DEBUG] New game save created!");
    }
    draw_ui_box(&game.box);

    draw_ui_box(&game.view_port);
    draw_ui_box(&game.control_surface);

    center_camera_on_player(&game.player, &game.camera, &game.view_port, &game.map);
    draw_map(&game.view_port);

    draw_player(&game.player, &game.camera, &game.view_port);

    draw_health_bar(game.player.health, &game.control_surface);

    draw_title_bar(term_w, &game.version);
    fflush(stdout);
}

void teleport_player(int x, int y) {
    game.camera.x = x - (game.view_port.width / 2);
    game.camera.y = y - (game.view_port.height / 2);

    if (game.camera.x < 0) {
        game.camera.x = 0;
        game.box.x = game.view_port.x + x - 1;
    } else if (game.camera.x > (game.map.width - game.view_port.width)) {
        int new_offset = game.camera.x - (game.map.width - game.view_port.width);
        game.camera.x = game.map.width - game.view_port.width;
        game.box.x = game.view_port.x + (game.view_port.width / 2) + new_offset;
    } else {
        game.box.x = game.view_port.x + (game.view_port.width / 2);
    }

    if (game.camera.y < 0) {
        game.camera.y = 0;
        game.box.y = game.view_port.y + y - 1;
    } else if (game.camera.y > (game.map.height - game.view_port.height)) {
        int new_offset = game.camera.y - (game.map.height - game.view_port.height);
        game.camera.y = game.map.height - game.view_port.height;
        game.box.y = game.view_port.y + (game.view_port.height / 2) + new_offset;
    } else {
        game.box.y = game.view_port.y + (game.view_port.height / 2);
    }

    draw_map(&game.view_port);
    draw_ui_box(&game.box);
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
            if (game.camera.y == game.map.height - game.view_port.height) {
                game.box.y = game.box.y + 1 > (game.view_port.height - game.box.height) ? game.view_port.height - game.box.height : game.box.y + 1;
            } else {
                game.box.y = game.box.y + 1 > vmax_h ? vmax_h : game.box.y + 1;

                if (game.box.py == game.box.y) game.camera.y++;
            }
        } else {
            game.box.y = game.box.y + 1 > (game.view_port.height - game.box.height) ? game.view_port.height - game.box.height : game.box.y + 1;
        }
    } else if (dir == UP) {
        game.box.py = game.box.y;
        if (game.camera.y == 0) {
            game.box.y = game.box.y - 1 > game.view_port.y + 1 ? game.box.y - 1 : game.view_port.y + 1;
        } else {
            game.box.y = game.box.y - 1 < vmin_h ? vmin_h : game.box.y - 1;
            if (game.box.py == game.box.y) game.camera.y--;
        }
    } else if (dir == RIGHT) {
        game.box.px = game.box.x;
        if (game.view_port.width < game.map.width) {
            if (game.camera.x == game.map.width - game.view_port.width) {
                game.box.x = game.box.x + 1 > (game.view_port.width - game.box.width) ? game.view_port.width - game.box.width : game.box.x + 1;
            } else {
                game.box.x = game.box.x + 1 > vmax_w ? vmax_w : game.box.x + 1;
                if (game.box.px == game.box.x) game.camera.x++;
            }
        } else {
            game.box.x = game.box.x + 1 > (game.view_port.width - game.box.width) ? game.view_port.width - game.box.width : game.box.x + 1;
        }
    } else {
        game.box.px = game.box.x;
        if (game.camera.x == 0) {
            game.box.x = game.box.x - 1 > game.view_port.x + 1 ? game.box.x - 1 : game.view_port.x + 1;
        } else {
            game.box.x = game.box.x - 1 < vmin_w ? vmin_w : game.box.x - 1;
            if (game.box.px == game.box.x) game.camera.x--;
        }
    }

    draw_map(&game.view_port);
    draw_ui_box(&game.box);

    draw_player(&game.player, &game.camera, &game.view_port);
    fflush(stdout);
}

void process_input(const char input) 
{    
    printf("\033[6;3H[DEBUG] Pressed Key: %c", input);

    switch (input)
    {
        case 119: // W
            step_player(UP);
            break;
        case 115: // S
            step_player(DOWN);
            break;
        case 97:  // A
            step_player(LEFT);
            break;
        case 100: // D
            step_player(RIGHT);
            break;
        case 105: // I Top Extreme Teleport.
            teleport_player(game.map.width / 2, 4);
            break;
        case 106: // J Left Extreme Teleport.
            teleport_player(4, game.map.height / 2);
            break;
        case 107: // K Bottom Extreme Teleport.
            teleport_player(game.map.width / 2, game.map.height - 4);
            break;
        case 108: // L Right Extreme Teleport.
            teleport_player(game.map.width - 4, game.map.height / 2);
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

    // Set terminal operating mode.
    set_conio_terminal_mode();

    // Start thread for keyboard input and resizing screen.
    iret2 = pthread_create( &thread2, NULL, draw_thread, NULL);
    iret1 = pthread_create( &thread1, NULL, blocking_keys, NULL);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);

    // Clear screen and reset terminal.
    clear_term();
    reset_terminal_mode();

    // Save the game.
    save_game(&game);

    printf("[DEBUG] Graceful exit. Thanks! :-)\n");
    return 0;
}
