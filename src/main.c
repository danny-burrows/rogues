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

Game game = {
    .version = "v0.0.6",
    .running = 0,
    .loaded = 0,
    .view_port = {1, 2, 1, 1, 0, 0, 0, 0},
    .control_surface = {1, 1, 1, 1, 0, 0, 0, 0},
    .player = {100, 20, 100.0f},
    .camera = {0},
    .map = {0}
};

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

    game.camera.vw = game.view_port.width; // Seems weird that these are the same :/
    game.camera.vh = game.view_port.height;

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

    draw_ui_box(&game.view_port);
    draw_ui_box(&game.control_surface);

    center_camera_on_player(&game.player, &game.camera, &game.map);
    draw_map(&game.view_port);

    player_draw(&game.player, &game.camera, &game.view_port);

    draw_health_bar(game.player.health, &game.control_surface);

    draw_title_bar(term_w, game.version);
    fflush(stdout);
}

void process_input(const char input) 
{    
    printf("\033[6;3H[DEBUG] Pressed Key: %c", input);

    switch (input)
    {
        case 119: // W
            // step_player(UP);
            player_step_up(&game.player, &game.camera, &game.map);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 115: // S
            // step_player(DOWN);
            player_step_down(&game.player, &game.camera, &game.map);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 97:  // A
            // step_player(LEFT);
            player_step_left(&game.player, &game.camera, &game.map);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 100: // D
            // step_player(RIGHT);
            player_step_right(&game.player, &game.camera, &game.map);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 105: // I Top Extreme Teleport.
            player_teleport(game.map.width / 2, 4, &game.player, &game.camera, &game.map, &game.view_port);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 106: // J Left Extreme Teleport.
            player_teleport(4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 107: // K Bottom Extreme Teleport.
            player_teleport(game.map.width / 2, game.map.height - 4, &game.player, &game.camera, &game.map, &game.view_port);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
            break;
        case 108: // L Right Extreme Teleport.
            player_teleport(game.map.width - 4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            draw_map(&game.view_port);
            player_draw(&game.player, &game.camera, &game.view_port);
            fflush(stdout);
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
        game.player.x = width / 2;
        game.player.y = height / 2;

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
