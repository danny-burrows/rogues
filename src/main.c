#include <math.h>
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

#define PI 3.14159

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
    .version = "v0.0.8",
    .running = 0,
    .loaded = 0,
    .day = 0,
    .time = 0,
    .view_port = {1, 2, 1, 1, 0, 0, 0, 0},
    .control_surface = {1, 1, 1, 1, 0, 0, 0, 0},
    .player = {20, 5, 100.0f},
    .camera = {0},
    .map = {0}
};

int prev_day = 0;
int daytime_counter = 0;
Draw_Buffer draw_buff;

int daytime_function(int x) {
    int speed_of_time = 30;
    int min_darkness = 30;

    // Controls the speed of a day :/
    float p = (float)x / speed_of_time;

    int time_val = 100 * sin(p) + 50;

    return time_val < min_darkness ? min_darkness : time_val;
}

int daytime_period(int x) {
    int speed_of_time = 30;
    int min_darkness = 30;

    // Controls the speed of a day :/
    float p = (float)x / speed_of_time;

    int time_val = 100 * sin(p) + 50;

    return time_val < min_darkness ? min_darkness : time_val;
}

void draw_debug_panel(void) {
    // Debug Menu
    printf("\033[2;3H─ DEBUG ─");
    printf("\033[3;3H| Game Version %s", game.version);
    
    if (game.loaded) {
        printf("\033[4;3H| Game save loaded successfully!");
    } else {
        printf("\033[4;3H| New game save created!");
    }
    
    printf("\033[5;3H| Viewport Size: %dx%d", game.view_port.width, game.view_port.height);
    printf("\033[6;3H| Player Pos: %d / %d  ", game.player.x, game.player.y);
    printf("\033[7;3H| Camera Pos: %d / %d  ", game.camera.x, game.camera.y);

    // Day & Time Values.
    int day = daytime_function(daytime_counter);
    int x_val = game.view_port.x + game.view_port.width - 16;
    printf("\033[2;%dH─ DAY AND TIME ─", x_val);
    printf("\033[3;%dH| Day: %d",     x_val, game.day);
    printf("\033[4;%dH| Day val: %d", x_val, day);
    printf("\033[5;%dH| Time: %.3f",  x_val, game.time);


    fflush(stdout);
}

void render_scene(Map * map, Camera * camera, Ui_Box * container) {

    Draw_Buffer_Copy(&draw_buff, map, camera->x, camera->y);

    // Apply lighting effects.
    for (int y = camera->y; y < camera->y + draw_buff.height; y++) {

        for (int x = camera->x; x < camera->x + draw_buff.width; x++) {

            // Attempt at a shader-like thing...
            double dist = sqrt((game.player.y - y) * (game.player.y - y) * 4 + (game.player.x - x) * (game.player.x - x));
            dist = dist / (double)sqrt((game.player.y - camera->y + camera->vh) * (game.player.y - camera->y + camera->vh) * 4 + (game.player.x - camera->x + camera->vw) * (game.player.x - camera->x + camera->vw));

            unsigned int *r = &draw_buff.data[y - camera->y][x - camera->x].r;
            unsigned int *g = &draw_buff.data[y - camera->y][x - camera->x].g;
            unsigned int *b = &draw_buff.data[y - camera->y][x - camera->x].b;

            // Day-Night with some distant fading.
            *r = *r * game.time * (1-dist);
            *g = *g * game.time * (1-dist);
            *b = *b * game.time * (1-dist);

            if (game.time < 0.45f) {
                // Nighttime torch and extra dark
                double mul = (1-5*dist) > 0 ? (1-5*dist) : 0;

                *r = *r * 4.0f * mul;
                *g = *g * 4.0f * mul;
                *b = *b * 4.0f * mul;

                // Night should be dark...
                *r = *r < 20 ? 0 : *r;
                *g = *g < 20 ? 0 : *g;
                *b = *b < 20 ? 0 : *b;

                // Red tinge from torch.
                if (dist < .15f) {
                   *r += (rand() % 200) * (mul/3);
                }

            }

            // Clap colour values at 255.
            *r = *r > 255 ? 255 : *r;
            *g = *g > 255 ? 255 : *g;
            *b = *b > 255 ? 255 : *b;


            *r = *r < 0 ? 0 : *r;
            *g = *g < 0 ? 0 : *g;
            *b = *b < 0 ? 0 : *b;
        }
    }

    // Drawing the player
    int player_rel_x = game.player.x - game.camera.x;
    int player_rel_y = game.player.y - game.camera.y;

    Draw_Buffer_AddString(&draw_buff, player_texture, player_rel_x, player_rel_y);    

    // The players stick!
    if (game.time < 0.45f) {
        draw_buff.data[player_rel_y][player_rel_x + 3].r = 225 + (rand() % 30);
        draw_buff.data[player_rel_y][player_rel_x + 3].g = 87;
        draw_buff.data[player_rel_y][player_rel_x + 3].b = 87;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].r = 150;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].g = 75;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].b = 85;
    } else {
        draw_buff.data[player_rel_y][player_rel_x + 3].r = 250;
        draw_buff.data[player_rel_y][player_rel_x + 3].g = 175;
        draw_buff.data[player_rel_y][player_rel_x + 3].b = 185;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].r = 150;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].g = 75;
        draw_buff.data[player_rel_y + 1][player_rel_x + 3].b = 85;
    }

    Draw_Buffer_Render(&draw_buff, 2, 3);

    draw_debug_panel();
}

void handle_resize(int term_w, int term_h) 
{
    game.view_port.width = term_w - 1;
    game.view_port.height = (.75f * term_h) - 2;

    game.camera.vw = game.view_port.width  - 1;
    game.camera.vh = game.view_port.height - 1;

    draw_buff.width  = game.camera.vw > DRAW_BUFFER_MAX_X ? DRAW_BUFFER_MAX_X : game.camera.vw;
    draw_buff.height = game.camera.vh > DRAW_BUFFER_MAX_Y ? DRAW_BUFFER_MAX_Y : game.camera.vh;

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

        daytime_counter++;

        game.time = (double)daytime_function(daytime_counter) / 100.0f;

        if (daytime_function(daytime_counter) != prev_day) render_scene(&game.map, &game.camera, &game.view_port);

        if (game.time < 0.45f && !(daytime_counter % 7)) render_scene(&game.map, &game.camera, &game.view_port);

        prev_day = daytime_function(daytime_counter);

        double sign_period = (2 * PI) / 0.033333f;
        game.day = (daytime_counter / sign_period) + 1;

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
    printf("\033[38;2;130;130;130m%s\033[0m\n", logo);

#ifdef DCONFIGSET
    d_printf(INFO, "Welcome Developer! Game launched in debug mode.\n\n");

    printf("=========== DEBUG KEY ===========\n");
    d_printf(INFO, "This is an info message.\n");
    d_printf(WARN, "This is a warning.\n");
    d_printf(ERR, " This is an error!\n");
    printf("=================================\n\n");
#endif

    // Load game...
    int r = load_game(&game);
    
    // If load failed and error is ENOENT (No such file or directory).
    if (r == -1 && errno == ENOENT) 
    {
        d_printf(INFO, "Attempting to load map & create new game...\n");
        if (Map_Load(&game.map, "./data/map/map.ansi") == -1 || save_game(&game) == -1) return EXIT_FAILURE;
    } 
    else if (r == -1) 
    {
        return EXIT_FAILURE;
    }

    printf("Press enter to begin...\n");
    getch();

    // Enter into alternative buffer.
    init_terminal(game.version);

    game.running = 1;

    // Set terminal operating mode.
    set_conio_terminal_mode();

    // Start thread for keyboard input and resizing screen.
    pthread_t drawing_thread, blocking_keys_thread;
    int iret2 = pthread_create(&drawing_thread, NULL, draw_thread, NULL);
    int iret1 = pthread_create(&blocking_keys_thread, NULL, blocking_keys, NULL);

    pthread_join(blocking_keys_thread, NULL);
    pthread_join(drawing_thread, NULL);

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
