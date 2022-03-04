#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "game.h"

#include "ui.h"
#include "map.h"
#include "debug.h"
#include "camera.h"
#include "player.h"
#include "drawing.h"
#include "terminal.h"
#include "saved_games.h"

/*
TODO: 
- Fix debug menu & proper debug interface!
- perror debug interface.
- Possibly temporary buffer for post processing.
- Lighting / Mock shaders!
- Buildings and interiors.
- Inventory system.
- NPC's and followers.
*/

static const char *logo = 
"  ▄████████  ▄██████▄     ▄██████▄   ███    █▄     ▄████████    ▄████████\n\
  ███    ███ ███    ███   ███    ███ ███    ███   ███    ███   ███    ███ \n\
  ███    ███ ███    ███   ███    █▀  ███    ███   ███    █▀    ███    █▀  \n\
 ▄███▄▄▄▄██▀ ███    ███  ▄███        ███    ███  ▄███▄▄▄       ███        \n\
▀▀███▀▀▀▀▀   ███    ███ ▀▀███ ████▄  ███    ███ ▀▀███▀▀▀     ▀███████████ \n\
▀███████████ ███    ███   ███    ███ ███    ███   ███    █▄           ███ \n\
  ███    ███ ███    ███   ███    ███ ███    ███   ███    ███    ▄█    ███ \n\
  ███    ███  ▀██████▀    ████████▀  ████████▀    ██████████  ▄████████▀  \n\
  ███    ███  Pre-Alpha                                                   \n";

Game game = {
    .version = "v0.0.7",
    .running = 0,
    .loaded = 0,
    .view_port = {1, 2, 1, 1, 0, 0, 0, 0},
    .control_surface = {1, 1, 1, 1, 0, 0, 0, 0},
    .player = {100, 20, 100.0f},
    .camera = {0},
    .map = {0}
};

Draw_Buffer draw_buff;

void draw_debug_panel(void) {
    // Debug Menu
    printf("\033[3;2H[INFO] Game Version %s", game.version);
    
    if (game.loaded) {
        printf("\033[4;2H[INFO] Game save loaded successfully!");
    } else {
        printf("\033[4;2H[INFO] New game save created!");
    }
    
    printf("\033[5;2H[INFO] Viewport Size: %dx%d", game.view_port.width, game.view_port.height);
    
    fflush(stdout);
}

void render_scene(Map * map, Camera * camera, Ui_Box * container) {

    Draw_Buffer_Fill(&draw_buff, map->data, camera->x, camera->y);

    player_draw(&game.player, &draw_buff, &game.camera, &game.view_port);

    Draw_Buffer_Display(&draw_buff, 2, 3);
}

void handle_resize(int term_w, int term_h) 
{
    game.view_port.width = term_w - 1;
    game.view_port.height = (.75f * term_h) - 2;

    draw_buff.w = game.view_port.width;
    draw_buff.h = game.view_port.height - 1;

    game.camera.vw = game.view_port.width; // Seems weird that these are the same :/
    game.camera.vh = game.view_port.height;

    game.control_surface.width = term_w - 1;
    game.control_surface.y = (.75f * term_h) + 1;
    game.control_surface.height = .25f * term_h - 1;

    clear_term();

    // Main UI Drawing
    draw_ui_box(&game.view_port);
    draw_ui_box(&game.control_surface);
    draw_health_bar(game.player.health, &game.control_surface);
    draw_title_bar(term_w, game.version);

    // Drawing Map
    camera_center_on_point(&game.camera, game.player.x, game.player.y, game.map.width, game.map.height);
    render_scene(&game.map, &game.camera, &game.view_port);

#ifdef DCONFIGSET 
    draw_debug_panel(); 
#endif
}

void process_input(const char input) 
{    
    switch (input)
    {
        case 119: // W
            player_step_up(&game.player, &game.camera, &game.map);
            break;
        case 115: // S
            player_step_down(&game.player, &game.camera, &game.map);
            break;
        case 97:  // A
            player_step_left(&game.player, &game.camera, &game.map);
            break;
        case 100: // D
            player_step_right(&game.player, &game.camera, &game.map);
            break;
        case 105: // I Top Extreme Teleport.
            player_teleport(game.map.width / 2, 4, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case 106: // J Left Extreme Teleport.
            player_teleport(4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case 107: // K Bottom Extreme Teleport.
            player_teleport(game.map.width / 2, game.map.height - 4, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case 108: // L Right Extreme Teleport.
            player_teleport(game.map.width - 4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case 27:
            // Empty any remaining chars in stdin...
            while (kbhit()){
                (void)getch();
            }

            // Program will now exit.
            game.running = 0;
            break;
        default:
            break;
    }

    render_scene(&game.map, &game.camera, &game.view_port);

#ifdef DCONFIGSET 
    draw_debug_panel(); 
#endif
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
            
        if (width != pw || height != ph) {
            pw = width;
            ph = height;

            // Size has changed so redraw the frame...
            handle_resize(width, height);
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

    printf("\033[38;2;130;130;130m%s\033[0m\n", logo);

#ifdef DCONFIGSET
    d_printf(INFO, "Welcome Developer! Game launched in debug mode.\n\n");

    printf("=========== DEBUG KEY ===========\n");
    d_printf(INFO, "This is a debug info message.\n");
    d_printf(WARN, "This is a warning.\n");
    d_printf(ERR,  "This is an error!\n");
    printf("=================================\n\n");
#endif

    // Load game...
    int r = load_game(&game);
    
    // If load failed and error is ENOENT (No such file or directory).
    if (r == -1 && errno == 2) 
    {
        d_printf(INFO, "Attempting to load map & create new game...\n");
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

    // Enter into alternative buffer.
    init_terminal(game.version);

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
    
    // Switch back to main buffer.
    reset_terminal();

    // Save the game.
    save_game(&game);

    d_printf(INFO, "Graceful exit. Thanks! :-)\n");
    return 0;
}
