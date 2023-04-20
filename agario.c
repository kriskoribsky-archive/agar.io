// IMPLEMENTATION of library "agario.h"
// AUTHOR: KRISTIAN KORIBSKY
// DATE: 8.12.2022
// ==========================================================================
#define _POSIX_C_SOURCE 200201L

#include "config.h"
#include "agario.h"
#include "name.h"

#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <math.h>
#include <limits.h>


// macros for easier color switching readability
#define COLOR_ON(x) attron(COLOR_PAIR(x))
#define COLOR_OFF(x) attroff(COLOR_PAIR(x))


void sleep_ms(const int milliseconds)
{
    struct timespec ts = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000
    };
    nanosleep(&ts, NULL);
}


int str_len(char *string)
{
    if (string == NULL)
        return -1;
    
    int c;
    for (c=0; *string != '\0'; c++)
        string++;   // increment pointer's adress by 1 which leads to a next element in a char array
    
    return c;
}


int rand_int(const int min, const int max)
{   
    // prevent floating-point exception (modulo by 0), also it is unexpected behaviour when min > max (err throwing would be nice)
    // if (min > max) return ERR;

    // rand returns value between 0 and maximum integer (in int range)
    return min + rand() % (max + 1 - min);
}


float distance(const int point1, const int point2)
{
    return sqrtf(point1*point1 + point2*point2);
}


bool inside_circle(const int row, const int col, const int center_row, const int center_col, const float radius)
{
    return distance(center_row-row, center_col-col) <= radius;
}


void render_circle(const int row, const int col, const float radius, const int color_pair)
{
    COLOR_OFF(BACKGROUND);
    COLOR_ON(color_pair);

    for (int i=row-radius; i <= row+radius; i++)
        for (int ii=col-radius; ii <= col+radius; ii++)
            if (inside_circle(i, ii, row, col, radius))
                mvprintw(i, ii, " ");
    
    COLOR_OFF(color_pair);
    COLOR_ON(BACKGROUND);
}


void render_number(const int row, const int col, char *format, const int num, const int color_pair)
{
    COLOR_OFF(BACKGROUND);
    COLOR_ON(color_pair);

    mvprintw(row, col, format, num);
    
    COLOR_OFF(color_pair);
    COLOR_ON(BACKGROUND);
}


void render_text(const int row, const int col, char *text, const int color_pair)
{
    COLOR_OFF(BACKGROUND);
    COLOR_ON(color_pair);

    mvprintw(row, col, "%s", text);

    COLOR_OFF(color_pair);
    COLOR_ON(BACKGROUND);
}


void render_string(const int row, const int col, char *format, char *string, const int color_pair)
{
    COLOR_OFF(BACKGROUND);
    COLOR_ON(color_pair);

    mvprintw(row, col, format, string);

    COLOR_OFF(color_pair);
    COLOR_ON(BACKGROUND);
}


float get_radius(const int size)
{
    return size / SIZE_MODIFIER + RADIUS_MODIFIER;
}


void init_screen(void)
{
    initscr();

    cbreak();                               // disable input buffering
    noecho();                               // do not echo getch() characters
    keypad(stdscr, TRUE);                   // enable function keys
    nodelay(stdscr, TRUE);                  // enables non-blocking getch()
    curs_set(FALSE);                        // always hide cursor

    mousemask(ALL_MOUSE_EVENTS, NULL);      // for getting mouse events
}


void init_colors(void)
{
    // inspired by https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/color.html
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color.\n");
        exit(EXIT_FAILURE);
    }

    start_color();

    init_pair(RED, COLOR_RED, COLOR_RED);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(GREEN, COLOR_GREEN, COLOR_GREEN);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(CYAN, COLOR_CYAN, COLOR_CYAN);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);

    init_pair(BACKGROUND, COLOR_WHITE, COLOR_WHITE);
    init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);

    init_pair(TEXT_CLR, COLOR_BLACK, COLOR_WHITE);
}


bool check_collision(const int row, const int col, const int radius, const int size, int world[size][size])
{
    if (world[row][col] != EMPTY) return FALSE;

    // here it is enough to do box-check, no need for circle-check
    for(int i=row-radius; i <= row+radius; i++)
        for (int ii=col-radius; ii <= col+radius; ii++)
            if (i != row && ii != col && world[i][ii] != EMPTY)
                return FALSE;

    return TRUE;
}


float entity_spawn(int *row, int *col, const int size, int world[size][size])
{
    // random spawn size
    int radius = rand_int(MIN_BASE_RADIUS, MAX_BASE_RADIUS);
    if (radius+1 > (size-1)-radius-1) return 0;     // radius can't be bigger than world size

    // try to generate TRIES number of times, then return 0 and don't spawn -> lots of collisions, world probably full
    int r, c;
    for (int i=0; i < TRIES; i++) {
        r = rand_int(radius+RADIUS_MODIFIER*2, (size-1)-radius-RADIUS_MODIFIER*2); // don't spawn very close to edge
        c = rand_int(radius+RADIUS_MODIFIER*2, (size-1)-radius-RADIUS_MODIFIER*2); // don't spawn very close to edge

        if (check_collision(r, c, radius, size, world)) {
            *row = r;
            *col = c;
            return radius;
        }
    }
    return 0;
}


void blob_spawn(int *blobs, const int max_blobs, const int size, int world[size][size])
{
    if (*blobs >= max_blobs)
        return;

    int row = rand_int(1, (size-1)-1);
    int col = rand_int(1, (size-1)-1);

    if(check_collision(row, col, BLOB_RADIUS+1, size, world)) {
        world[row][col] = rand_int(ENTITY_COLORS_START, ENTITY_COLORS_END);
        *blobs += 1;
    }
}

void update_bot_vectors(const int n, const int params, int ent[n][params], const float difficulty)
{
    // player position
    int p_row = ent[PLAYER][ROW];
    int p_col = ent[PLAYER][COL];

    // loop trough living bots, not counting player
    for (int i=PLAYER+1; i < n; i++) {
        if (ent[i][ALIVE] == FALSE)
            continue;

        // entity position
        int e_row = ent[i][ROW];
        int e_col = ent[i][COL];
        
        // when bot is too far away from player (not in viewport) - calculate vectors randomly
        if (abs(e_row - p_row) > LINES/2 - LINES*0.1 || abs(e_col - p_col) > COLS/2 - COLS*0.1) {
            ent[i][ROW_VECTOR] = rand_int(-VERTICAL_MODIFIER, VERTICAL_MODIFIER);
            ent[i][COL_VECTOR] = rand_int(-HORIZONTAL_MODIFIER, HORIZONTAL_MODIFIER);
            continue;
        }
        // relative vectors pointing to player
        int relative_row_vector = e_row > p_row ? -VERTICAL_MODIFIER : VERTICAL_MODIFIER;      
        int relative_col_vector = e_col > p_col ? -HORIZONTAL_MODIFIER : HORIZONTAL_MODIFIER;

        // calculate whether bot should chase or run away
        int chase = ent[i][SIZE] >= ent[PLAYER][SIZE] ? 1 : -1;
        int outcome = difficulty * 100 > rand_int(0, 100) ? chase : chase * -1;

        ent[i][ROW_VECTOR] = outcome * relative_row_vector;
        ent[i][COL_VECTOR] = outcome * relative_col_vector; 
    }
}


void update_player_vectors(const int ch, int *row_vector, int *col_vector)
{
    MEVENT event;

    if (getmouse(&event) == OK) {
        // each dimension is divided into thirds, which creates 6 possible vectors (N, NE, E, SE, S, SW, W, NW)
        int horizontal_third = LINES / 3;
        int vertical_third = COLS / 3;

        // row vector
        if (event.y < horizontal_third)
            *row_vector = -VERTICAL_MODIFIER;
        else if (event.y > horizontal_third && event.y < 2*horizontal_third)
            *row_vector = 0;
        else 
            *row_vector = VERTICAL_MODIFIER;
        
        // col vector
        if (event.x < vertical_third)
            *col_vector = -HORIZONTAL_MODIFIER;
        else if (event.x > vertical_third && event.x < 2*vertical_third)
            *col_vector = 0;
        else
            *col_vector = HORIZONTAL_MODIFIER;
    } else {
        switch (ch) {
            case KEY_UP:
                *row_vector = -VERTICAL_MODIFIER;
                *col_vector = 0;
                break;
            
            case KEY_RIGHT:
                *row_vector = 0;
                *col_vector = HORIZONTAL_MODIFIER;
                break;

            case KEY_DOWN:
                *row_vector = VERTICAL_MODIFIER;
                *col_vector = 0;
                break;

            case KEY_LEFT:
                *row_vector = 0;
                *col_vector = -HORIZONTAL_MODIFIER;
                break;
            
            case ' ':
                *row_vector = 0;
                *col_vector = 0;
        }
    }
}


void update_positions(const int n, const int params, int ent[n][params], const int size, int world[size][size])
{
    // iterate entity register
    for (int i = 0; i < n; i++) {
        if (ent[i][ALIVE] == FALSE)
            continue;

        // border checking
        int radius = get_radius(ent[i][SIZE]);
        int new_row = ent[i][ROW] + ent[i][ROW_VECTOR];
        int new_col = ent[i][COL] + ent[i][COL_VECTOR];

        // need to split these condtitions because of custom move speed modifiers
        if (new_row - radius < 0)
            new_row = radius;
        else if (new_row + radius >= size)
            new_row = (size-1)-radius;
        
        if (new_col - radius < 0)
            new_col = radius;
        else if (new_col + radius >= size)
            new_col = (size-1)-radius;

        // update entity in world
        world[ent[i][ROW]][ent[i][COL]] = EMPTY;
        world[new_row][new_col] = ENTITY_START + i;

        // update ent in entities registry
        ent[i][ROW] = new_row;
        ent[i][COL] = new_col;   
    }
}


void eval_positions(const int n, const int params, int ent[n][params], const int size, int world[size][size], int *blobs, int *alive)
{
    *alive = 0;     // reset counter before evaluating all entities
    
    // iterate entity register
    for (int k=0; k < n; k++) {
        if (ent[k][ALIVE] == FALSE)
            continue;
        
        *alive += 1;

        int row = ent[k][ROW];
        int col = ent[k][COL];
        float radius = get_radius(ent[k][SIZE]);

        // get bigger of the two modifiers to account for that in hitboxes (to prevent jumping over the hitbox)
        int modiff = HORIZONTAL_MODIFIER > VERTICAL_MODIFIER ? HORIZONTAL_MODIFIER : VERTICAL_MODIFIER;
        modiff -= 1;    // base modifier starts at 1

        // collision evalutation algorithm: iterate & evaluate every index along the circle's circumference
        for (int i=row-radius-modiff; i <= row+radius+modiff; i++) {
            for (int ii=col-radius-modiff; ii <= col+radius+modiff; ii++) {
                float d = distance(row - i, col - ii);

                if (d >= radius-modiff-1 && d <= radius) {
                    if (world[i][ii] == EMPTY) {
                        continue;
                        
                    } else if (world[i][ii] >= BLOB_START && world[i][ii] < ENTITY_START) {
                        ent[k][SIZE] += 1;
                        *blobs -= 1;
                        world[i][ii] = EMPTY;
                    
                    // if current's entity circumference reached centre of other entity before the other did so,
                    // radius and thus size of current entity is bigger, therefore eliminate given entity
                    // if the 2 radii are equal, nothing happens
                    } else if (world[i][ii] >= ENTITY_START) {
                        if (get_radius(ent[k][SIZE]) > get_radius(ent[world[i][ii] - ENTITY_START][SIZE])) {
                            ent[world[i][ii] - ENTITY_START][ALIVE] = FALSE;
                            ent[k][SIZE] += ent[world[i][ii] - ENTITY_START][SIZE] * GROW_MODIFIER;
                            world[i][ii] = EMPTY;
                        }
                    }
                }
            }
        }
    }
}


bool heading_menu(char *head, char *option1, char *option2)
{
    int head_len = str_len(head);
    int len1 = str_len(option1);
    int len2 = str_len(option2);

    // create new window
    int menu_lines = 10;
    int menu_cols = head_len * 1.4;      // heading will be the longest text

    // centers of window
    int center_h = menu_cols / 2;
    int center_v = menu_lines / 2;

    WINDOW *w = newwin(menu_lines, menu_cols, LINES / 2 - center_v, COLS / 2 - center_h); // place window in the center of stdscr

    // window properties
    keypad(w, TRUE);
    nodelay(w, TRUE);

    box(w, 0, 0);   // border (default)

    int p = 1;      // padding

    // heading
    wattron(w, A_BOLD);
    mvwprintw(w, 1 + p, center_h - head_len / 2, "%s", head);
    wattroff(w, A_BOLD);

    // render body for the first time
    wattron(w, A_STANDOUT);
    mvwprintw(w, 4 + p, center_h - len1 / 2, "%s", option1);
    wattroff(w, A_STANDOUT);
    mvwprintw(w, 5 + p, center_h - len2 / 2, "%s", option2);

    // body arrows select
    int option = 0;
    int ch;
    while ((ch = wgetch(w)) != SUBMIT_KEY && ch != MENU_KEY) {      
        // re-render elements
        switch (ch) {
            case KEY_UP:
                option = 0;
                wattron(w, A_STANDOUT);
                mvwprintw(w, 4 + p, center_h - len1/2, "%s", option1);
                wattroff(w, A_STANDOUT);
                mvwprintw(w, 5 + p, center_h - len2/2, "%s", option2);
                break;

            case KEY_DOWN:
                option = 1;
                mvwprintw(w, 4 + p, center_h - len1/2, "%s", option1);
                wattron(w, A_STANDOUT);
                mvwprintw(w, 5 + p, center_h - len2/2, "%s", option2);
                wattroff(w, A_STANDOUT);
                break;
        }
        wrefresh(w);
    }
    delwin(w);  // free memory

    if (ch == SUBMIT_KEY && option == 0)
        return TRUE;
    
    return FALSE;
}


int menu(char *head, char *opt1, char *opt2, char *opt3)
{   
    int menu_lines = 6;
    int menu_cols = str_len(head) * 1.4;

    int center_h = menu_cols / 2;
    int center_v = menu_lines / 2;

    WINDOW *w = newwin(menu_lines, menu_cols, LINES / 2 - center_v, COLS / 2 - center_h);

    // window properties
    keypad(w, TRUE);
    nodelay(w, TRUE);

    box(w, 0, 0);

    int p = 1;

    // heading
    wattron(w, A_BOLD);
    mvwprintw(w, p, center_h - str_len(head) / 2, "%s", head);
    wattroff(w, A_BOLD);

    char *options[3] = {opt1, opt2, opt3};
    int option = 0;
    int ch;
    while ((ch = wgetch(w)) != SUBMIT_KEY && ch != MENU_KEY) {
        switch(ch) {
            case KEY_UP:
                option = option-1 < 0 ? option : option-1;
                break;
            
            case KEY_DOWN:
                option = option+1 > 2 ? option : option+1;
                break;
        }

        // render options
        for (int i=0; i < 3; i++) {
            if (option == i) {
                wattron(w, A_STANDOUT);
                mvwprintw(w, p + i + 1, center_h - str_len(options[i])/2, "%s", options[i]);
                wattroff(w, A_STANDOUT);
            } else {
                mvwprintw(w, p + i + 1, center_h - str_len(options[i])/2, "%s", options[i]);
            }
        }
        wrefresh(w);
    }

    delwin(w);

    if (ch == MENU_KEY)
        return 0;
    
    return option+1;
}


void input_menu(char *label, const int input_len, char input[input_len])
{
    int label_len = str_len(label);

    int menu_lines = 3;
    int menu_cols = (input_len+label_len) * 1.4;

    int center_h = menu_cols / 2;
    int center_v = menu_lines / 2;

    WINDOW *w = newwin(menu_lines, menu_cols, LINES / 2 - center_v, COLS / 2 - center_h);

    box(w, 0, 0);

    mvwprintw(w, 1, 2, "%s", label);
    echo();
    wscanw(w, "%s", input);
    noecho();
    wrefresh(w);

    input[input_len-1] = '\0';    // add last null terminator
    delwin(w);
}


void render_viewport(const int n, const int params, int ent[n][params], const int len, char ent_names[n][len], const int size, int world[size][size], const int bots)
{
    // relative upper-left corner of world to viewport
    int y = ent[PLAYER][ROW] - LINES/2;
    int x = ent[PLAYER][COL] - COLS/2;

    // entities buffer because entities need to be drawn after the background
    int render_buffer[n][3];     // 0 -> index, 1 -> world row, 2 -> world col = 3
    int buffer_i = 0;

    // viewport is always centered on player
    // render background & save blobs & entities to buffer
    for (int i=0; i < LINES; i++)
            for (int ii=0; ii < COLS; ii++)
                // check if viewport is inside world
                if (y+i >= 0 && y+i < size && x+ii >= 0 && x+ii < size) {
                    int index = world[y+i][x+ii];
                    // empty
                    if (index == EMPTY) {
                        mvprintw(i, ii, " ");
                    // blob
                    } else if (index >= BLOB_START && index < ENTITY_START) {
                        render_circle(i, ii, BLOB_RADIUS, index);       // render blob here since it will always have size of 1
                    // entity
                    } else {
                        // save entity to buffer to be rendered later
                        if (ent[index - ENTITY_START][ALIVE]) {
                            render_buffer[buffer_i][0] = index;
                            render_buffer[buffer_i][1] = i;
                            render_buffer[buffer_i][2] = ii;
                            buffer_i++;
                        }
                    }
                // if viewport outside of the world, render black pixels
                } else {
                    render_text(i, ii, " ", BLACK);
                }
    
    // render blobs & entities
    for (int i=0; i < buffer_i; i++) {
        int index = render_buffer[i][0];
        int row = render_buffer[i][1];
        int col = render_buffer[i][2];
        render_circle(row, col, get_radius(ent[index - ENTITY_START][SIZE]), ent[index - ENTITY_START][COLOR]);

        int radius = get_radius(ent[index - ENTITY_START][SIZE]);
        render_string(row - radius - 2, col - str_len(ent_names[index - ENTITY_START])/2, "%s", ent_names[index - ENTITY_START], TEXT_CLR);
    }

    // game state info
    render_number(LINES-2, 0, "ENEMIES LEFT: %d", bots, TEXT_CLR);
    render_number(LINES-1, 0, "YOUR SIZE: %d", ent[PLAYER][SIZE], TEXT_CLR);

    // IMPORTANT TO CALL CURSES' REFRESH FOR UPDATES
    refresh();
}


void agario(const int world_size, const int max_bots)
{
    // Init
    // ==========================================================================
    init_screen();
    init_colors();

    newgame:
    // init world
    int world[world_size][world_size];
    for (int i=0; i < world_size; i++)
        for (int ii=0; ii < world_size; ii++)
            world[i][ii] = EMPTY;

    // init entities
    int line, max_length;
    int ent_count = max_bots+PLAYERS;
    random_line(&line, &max_length);
    char ent_names[ent_count][max_length];

    int ent_alive = 0;
    int ent_row, ent_col, ent_radius;
    int ent[ent_count][PARAMS];
    for (int i=0; i < ent_count; i++) {
        ent_radius = entity_spawn(&ent_row, &ent_col, world_size, world);

        ent[i][ROW] = ent_row;
        ent[i][COL] = ent_col;
        ent[i][ROW_VECTOR] = 0;
        ent[i][COL_VECTOR] = 0;
        ent[i][SIZE] = ent_radius * SIZE_MODIFIER;
        ent[i][ALIVE] = ent[i][SIZE] > 0 ? TRUE : FALSE;
        ent[i][COLOR] = rand_int(ENTITY_COLORS_START, ENTITY_COLORS_END);

        // add living entity to world marked with its unique index starting from ENTITY START (player is at ENTITY_START)
        if (ent[i][ALIVE]) {
            world[ent[i][ROW]][ent[i][COL]] = ENTITY_START + i;
            ent_alive++;
        }

        // entity name
        random_line(&line, &max_length);
        get_line(line, max_length, ent_names[i]);
    }
    ent_names[PLAYER][0] = '\0';

    // init blobs
    int blobs = 0;
    int blobs_max = world_size / BLOB_MAX_RATIO + 1;
    // spawn more blobs at once in the beggining
    for (int i=0; i < blobs_max-1; i++)
        blob_spawn(&blobs, blobs_max, world_size, world);

    // Menus
    // ==========================================================================
    // first time menu - displays already generated world in the background
    COLOR_ON(BACKGROUND);
    render_viewport(max_bots + PLAYERS, PARAMS, ent, max_length, ent_names, world_size, world, ent_alive - PLAYERS);
    COLOR_OFF(BACKGROUND);

    // intial menu & user response
    int res = heading_menu(WELCOME_TEXT, PLAY_GAME_TEXT, EXIT_TEXT);
    if(res == FALSE) {
        endwin();
        return;
    }
    
    // re-render map between menu change
    COLOR_ON(BACKGROUND);
    render_viewport(max_bots + PLAYERS, PARAMS, ent, max_length, ent_names, world_size, world, ent_alive - PLAYERS);
    COLOR_OFF(BACKGROUND);

    // get name from user
    input_menu(NICKNAME_LABEL_TEXT, MAX_NICKNAME_LEN, ent_names[PLAYER]);

    // re-render map between menu change
    COLOR_ON(BACKGROUND);
    render_viewport(max_bots + PLAYERS, PARAMS, ent, max_length, ent_names, world_size, world, ent_alive - PLAYERS);
    COLOR_OFF(BACKGROUND);

    // get bot difficulty level
    int option = menu(BOT_HEADING_TEXT, BOT_EASY_TEXT, BOT_MEDIUM_TEXT, BOT_HARD_TEXT);
    double bot_difficulty = BOT_HARD;

    switch(option) {
        case 1:
            bot_difficulty = BOT_EASY;
            break;
        
        case 2:
            bot_difficulty = BOT_MEDIUM;
            break;
        
        case 3:
            bot_difficulty = BOT_HARD;
            break;
    }

    // Game loop
    // ==========================================================================
    gameloop:
    COLOR_ON(BACKGROUND);
    int end_delay = END_DELAY * 1000 / TICK_RATE;                 
    int ch;
    unsigned long ticks = 0;
    while ((ch = getch()) != MENU_KEY && end_delay > 0) {

        if (ticks % BLOB_UPDATE_RATE == 0)
            blob_spawn(&blobs, blobs_max, world_size, world);

        if (ticks % VECTOR_UPDATE_RATE == 0)
            update_bot_vectors(ent_count, PARAMS, ent, bot_difficulty);
                
        if (ch != ERR)
            update_player_vectors(ch, &ent[PLAYER][ROW_VECTOR], &ent[PLAYER][COL_VECTOR]);

        update_positions(ent_count, PARAMS, ent, world_size, world);
        eval_positions(ent_count, PARAMS, ent, world_size, world, &blobs, &ent_alive);
        render_viewport(ent_count, PARAMS, ent, max_length, ent_names, world_size, world, ent_alive-PLAYERS);
        
        // game end delay
        if (!ent[PLAYER][ALIVE] || ent_alive <= PLAYERS)
            end_delay--;

        ticks = ticks >= ULONG_MAX - 1 ? 0 : ticks+1; // update tick & overflow protection
        sleep_ms(TICK_RATE);
    }

    // Game pause || End of the game
    // ==========================================================================
    bool new_game = FALSE;

    // menu 
    if (ch == MENU_KEY) 
        if (heading_menu(GAME_RUNNING_TEXT, CONTINUE_GAME_TEXT, EXIT_GAME_TEXT))
            goto gameloop;
        else
            new_game = TRUE;
    // game end
    else  {
        char *message = ent[PLAYER][ALIVE] ? GAME_WON_TEXT : GAME_LOST_TEXT;
        
        if (heading_menu(message, NEW_GAME_TEXT, EXIT_TEXT))
            new_game = TRUE;
    }

    if(new_game)
        goto newgame;

    endwin();   // de-init window on exit
}
