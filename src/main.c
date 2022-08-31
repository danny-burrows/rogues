#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "game.h"

#include "ui.h"
#include "map.h"
#include "keys.h"
#include "debug.h"
#include "camera.h"
#include "player.h"
#include "drawing.h"
#include "terminal.h"
#include "saved_games.h"

#define PI 3.14159f

/*
TODO: 
- perror debug interface.
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
    .map = {0},
    .render_request = RENDER_REQUEST_GAME_STARTUP
};

double TIME_FOR_60_FRAMES;
double TIME_FOR_FRAME;
double FPS;

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

    int dbg_x = game.control_surface.x + 2;
    int dbg_y = game.control_surface.y;

    #define POS_PRINTF(x, y, ...) {printf("\033[%u;%uH", y, x);printf(__VA_ARGS__);}

    // Debug Menu
    POS_PRINTF(dbg_x, dbg_y, " DEBUG ");
    POS_PRINTF(dbg_x, dbg_y + 1, "Game Version %s", game.version);
    
    if (game.loaded) {
        POS_PRINTF(dbg_x, dbg_y + 2, "Game save loaded successfully!");
    } else {
        POS_PRINTF(dbg_x, dbg_y + 2, "New game save created!");
    }
    
    // Leaving extra space to cover format overflows on || large numbers
    //                                                  \/ 
    POS_PRINTF(dbg_x, dbg_y + 3, "Viewport Size: %dx%d     ", game.view_port.width, game.view_port.height);
    POS_PRINTF(dbg_x, dbg_y + 4, "Player Pos:    %d / %d   ", game.player.x, game.player.y);
    POS_PRINTF(dbg_x, dbg_y + 5, "Camera Pos:    %d / %d   ", game.camera.x, game.camera.y);
    
    POS_PRINTF(dbg_x + 40, dbg_y + 1, "FPS:                 %.2f    ", FPS);
    POS_PRINTF(dbg_x + 40, dbg_y + 2, "FRAME RENDER TIME:   %.2fs   ", TIME_FOR_FRAME);
    POS_PRINTF(dbg_x + 40, dbg_y + 3, "TIME FOR 60 FRAMES:  %.2fs   ", TIME_FOR_60_FRAMES);
    POS_PRINTF(dbg_x + 40, dbg_y + 4, "Last Render Request: %d      ", game.render_request);

    // Day & Time Values.
    int day = daytime_function(daytime_counter);
    int x_val = game.view_port.x + game.view_port.width - 26;

    POS_PRINTF(x_val, dbg_y, " DAY AND TIME ");
    POS_PRINTF(x_val, dbg_y + 1, "Day: %d", game.day);
    POS_PRINTF(x_val, dbg_y + 2, "Day val: %3.d", day);
    POS_PRINTF(x_val, dbg_y + 3, "Time: %.3f", game.time);

    fflush(stdout);
}

typedef struct _Point {
    int x;
    int y;
} Point;

typedef struct _Wall {
    Point begin;
    Point end;
} Wall;

bool isPointBetweenPoints(Point currPoint, Point point1, Point point2)
{
    int dxc = currPoint.x - point1.x;
    int dyc = currPoint.y - point1.y;

    int dxl = point2.x - point1.x;
    int dyl = point2.y - point1.y;

    int cross = dxc * dyl - dyc * dxl;

    double threshold = 2.0; 
    if (abs((double)cross) > threshold) 
        return false;

    // For an exact match to the line
    // if (cross != 0)
    //     return false;

    if (abs(dxl) >= abs(dyl))
    return dxl > 0 ? 
        point1.x <= currPoint.x && currPoint.x <= point2.x :
        point2.x <= currPoint.x && currPoint.x <= point1.x;
    else
    return dyl > 0 ? 
        point1.y <= currPoint.y && currPoint.y <= point2.y :
        point2.y <= currPoint.y && currPoint.y <= point1.y;
}

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= MAX(p.x, r.x) && q.x >= MIN(p.x, r.x) &&
		q.y <= MAX(p.y, r.y) && q.y >= MIN(p.y, r.y))
	return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0; // collinear

	return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are collinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are collinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are collinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are collinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

void render_scene(Map * map, Camera * camera, Ui_Box * container) {

    Draw_Buffer_Copy(&draw_buff, map, camera->x, camera->y);

    // Apply lighting effects.
    for (int y = camera->y; y < camera->y + draw_buff.height; y++) {

        for (int x = camera->x; x < camera->x + draw_buff.width; x++) {

            // Attempt at a shader-like thing...
            double dist = sqrt((game.player.y - y) * (game.player.y - y) * 4 + (game.player.x - x) * (game.player.x - x));
            dist = dist / (double)sqrt((game.player.y - camera->y + camera->vh) * (game.player.y - camera->y + camera->vh) * 4 + (game.player.x - camera->x + camera->vw) * (game.player.x - camera->x + camera->vw));

            Pixel *pxl = &draw_buff.data[y - camera->y][x - camera->x];
            
            Wall walls[8] = {
                {
                    {.x = 40, .y = 10},
                    {.x = 54, .y = 10}
                },
                {
                    {.x = 40, .y = 20},
                    {.x = 54, .y = 20}
                },
                {
                    {.x = 49, .y = 5},
                    {.x = 49, .y = 10}
                },
                {
                    {.x = 49, .y = 20},
                    {.x = 49, .y = 25}
                },
                {
                    {.x = 49, .y = 5},
                    {.x = 80, .y = 5}
                },
                {
                    {.x = 49, .y = 25},
                    {.x = 80, .y = 25}
                },
                {
                    {.x = 80, .y = 5},
                    {.x = 80, .y = 25}
                },
            };

            for (int i = 0; i < 8; i++) {
                Point player_point, current_point;

                player_point.x  = game.player.x;
                player_point.y  = game.player.y;
                current_point.x = x;
                current_point.y = y;

                // Wall is blocking our view of current_point
                if (doIntersect(walls[i].begin, walls[i].end, player_point, current_point)) {
                    pxl->r = 0;
                    pxl->g = 0;
                    pxl->b = 0;
                }

                // Current point is a wall
                if (isPointBetweenPoints(current_point, walls[i].begin, walls[i].end)) {
                    pxl->r = 164;
                    pxl->g = 70;
                    pxl->b = 104;
                }
            }

            // Day-Night with some distant fading.
            pxl->r = pxl->r * game.time * (1-dist);
            pxl->g = pxl->g * game.time * (1-dist);
            pxl->b = pxl->b * game.time * (1-dist);

            if (game.time < 0.45f) {
                // Nighttime torch and extra dark
                double mul = (1-5*dist) > 0 ? (1-5*dist) : 0;

                pxl->r = pxl->r * 4.0f * mul;
                pxl->g = pxl->g * 4.0f * mul;
                pxl->b = pxl->b * 4.0f * mul;

                // Night should be dark...
                pxl->r = pxl->r < 20 ? 0 : pxl->r;
                pxl->g = pxl->g < 20 ? 0 : pxl->g;
                pxl->b = pxl->b < 20 ? 0 : pxl->b;

                // Red tinge from torch.
                if (dist < .15f) {
                   pxl->r += (rand() % 200) * (mul/3);
                }

            }

            // Clap colour values at 255.
            pxl->r = pxl->r > 255 ? 255 : pxl->r;
            pxl->g = pxl->g > 255 ? 255 : pxl->g;
            pxl->b = pxl->b > 255 ? 255 : pxl->b;


            pxl->r = pxl->r < 0 ? 0 : pxl->r;
            pxl->g = pxl->g < 0 ? 0 : pxl->g;
            pxl->b = pxl->b < 0 ? 0 : pxl->b;

            // Inerpolate new colors for background.
            pxl->bg_r = pxl->r / 3;
            pxl->bg_g = pxl->g / 3;
            pxl->bg_b = pxl->b / 3;
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

    // TODO: This should probably not be here...
#ifdef DCONFIGSET 
    draw_debug_panel(); 
#endif
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
    
    game.render_request = RENDER_REQUEST_WINDOW_RESIZE;
}

void process_input(const char input) 
{
    switch (input)
    {
        case KEY_W:
            player_step_up(&game.player, &game.camera, &game.map);
            break;
        case KEY_A:
            player_step_left(&game.player, &game.camera, &game.map);
            break;
        case KEY_S:
            player_step_down(&game.player, &game.camera, &game.map);
            break;
        case KEY_D:
            player_step_right(&game.player, &game.camera, &game.map);
            break;
        case KEY_I: // Top Extreme Teleport.
            player_teleport(game.map.width / 2, 4, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case KEY_J: // Left Extreme Teleport.
            player_teleport(4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case KEY_K: // Bottom Extreme Teleport.
            player_teleport(game.map.width / 2, game.map.height - 4, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case KEY_L: // Right Extreme Teleport.
            player_teleport(game.map.width - 4, game.map.height / 2, &game.player, &game.camera, &game.map, &game.view_port);
            break;
        case KEY_Q:
        case KEY_ESC:
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

    game.render_request = RENDER_REQUEST_PLAYER_INPUT;
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

        if (daytime_function(daytime_counter) != prev_day || 
                game.time < 0.45f && !(daytime_counter % 7)) 
            game.render_request = RENDER_REQUEST_DAYTIME_UPDATE;

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
    while (game.running)
        // Block and wait to read from stdin.
        process_input(getch());
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

    // Main draw loop.
    struct timeval fps_time_start, fps_time_end;

    int frame_count = 0;
    const int FPS_INTERVAL = 60;
    gettimeofday(&fps_time_start, NULL);
    while (game.running) {

        // Check for draw calls at a max of 60fps
        while (game.render_request == RENDER_REQUEST_NOT_REQUESTED) {
            usleep(1000000 / 60);
        }
        game.render_request = RENDER_REQUEST_NOT_REQUESTED;

        // Calculate live FPS 
        if (frame_count++ >= FPS_INTERVAL) {
            gettimeofday(&fps_time_end, NULL);

            long seconds = (fps_time_end.tv_sec - fps_time_start.tv_sec);
            TIME_FOR_60_FRAMES = ((double)(seconds * 1000000) + fps_time_end.tv_usec) - (fps_time_start.tv_usec);
            TIME_FOR_60_FRAMES /= 1000000.0;
            TIME_FOR_FRAME = TIME_FOR_60_FRAMES / frame_count;
            FPS = (double)frame_count / TIME_FOR_60_FRAMES;

            frame_count = 0;
            gettimeofday(&fps_time_start, NULL);
        }

        render_scene(&game.map, &game.camera, &game.view_port);
    }

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
