// CONFIGURATION file for "agario.h" library
// AUTHOR: KRISTIAN KORIBSKY
// DATE: 8.12.2022
// ==========================================================================
// game
#define GAME_PARAMETERS 2
#define MIN_WORLD_SIZE 100
#define MAX_WORLD_SIZE 1200
#define MIN_BOT_COUNT 0
#define MAX_BOT_COUNT 1000

#define MENU_KEY KEY_BACKSPACE  // curses key for pausing/displaying menu
#define SUBMIT_KEY '\n'         // key for submitting menu option

// menus
#define MAX_NICKNAME_LEN 10     // limits username length to this number

#define WELCOME_TEXT "Welcome to the Agar.io!"
#define GAME_RUNNING_TEXT "You paused the game"
#define GAME_WON_TEXT "Congratulations, you've won!"
#define GAME_LOST_TEXT "Unfortunately, you lost!"
#define PLAY_GAME_TEXT "Play"
#define NEW_GAME_TEXT "New game"
#define EXIT_GAME_TEXT "Exit game"
#define CONTINUE_GAME_TEXT "Continue"
#define EXIT_TEXT "Exit"

#define NICKNAME_LABEL_TEXT "Enter your name: "

#define BOT_HEADING_TEXT "Choose bot difficulty:"
#define BOT_EASY_TEXT "EASY"
#define BOT_MEDIUM_TEXT "MEDIUM"
#define BOT_HARD_TEXT "HARD"

// timing
#define TICK_RATE 60            // miliseconds between game updates
#define BLOB_UPDATE_RATE 20     // game ticks to wait after spawning new blob
#define VECTOR_UPDATE_RATE 5    // game ticks to wait after updating bot directions
#define END_DELAY 2             // how many seconds to wait until game will end after winning/loosing

// generator
#define PLAYERS 1               // number of players (current implementation supports only 1 player)
#define TRIES 2                 // number of times generator will try to randomly spawn entity (after that the world is probably full)
#define BLOB_MAX_RATIO 2        // ratio to determine maximum number of blobs existing at the same time

// world
#define EMPTY 0                 // empty position in the world
#define BLOB_START 1            // each blob will have its color represented by index
#define ENTITY_START 10         // players & bots identified by index

// movement
#define HORIZONTAL_MODIFIER 2   // modifies horizontal movement speed
#define VERTICAL_MODIFIER 1     // modifies vertical movement speed

// sizes
#define BLOB_RADIUS 0           // size of a blob (current implementation supports only 0)
#define MIN_BASE_RADIUS 2       // min random starting size of a player/bot
#define MAX_BASE_RADIUS 6       // max random starting size of a player/bot
#define RADIUS_MODIFIER 0.5     // circles are better looking with .5 radius values
#define SIZE_MODIFIER 10        // amount of blobs needed to increase radius by 1
#define GROW_MODIFIER 0.5       // how much size increases after consuming other entity's size

// entities = players & bots
#define PARAMS 7                // number of parameters stored for each entity
#define ROW 0
#define COL 1
#define ROW_VECTOR 2
#define COL_VECTOR 3
#define SIZE 4
#define ALIVE 5
#define COLOR 6

#define PLAYER 0                // player's index in entities & world array

// bots
// probabilty of bot chasing/running away from player according to its advantage/disadvantage
#define BOT_EASY 0.3
#define BOT_MEDIUM 0.7
#define BOT_HARD 0.98  

// colors
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define CYAN 5
#define MAGENTA 6

#define ENTITY_COLORS_START 1       // first allowed entity color
#define ENTITY_COLORS_END 6         // last allowed entity color

#define BACKGROUND 7                // white
#define BLACK 8
#define TEXT_CLR 9                  // info panel color pair
