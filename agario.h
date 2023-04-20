// LIBRARY "agario.h"
// AUTHOR: KRISTIAN KORIBSKY
// DATE: 8.12.2022
// ==========================================================================
#include <stdbool.h>


/**
 * Compact UNIX sleep function.
 * Pauses main thread execution for miliseconds amount of time.
 * @implements nanosleep() from <time.h>
 * @param miliseconds amount of time to pause for
 * 
*/
void sleep_ms(const int miliseconds);


/**
 * Calculates the length of a string.
 * @attention given string needs to be terminated with '\0' terminator
 * @param string pointer to a char (array)
 * @returns -1 if pointer is NULL, otherwise length of a string
*/
int str_len(char *string);


/**
 * Generates pseudo-random int from the given incluvive range.
 * I used this simple algorithm from ballsortpuzzle problem set.
 * @param min minimum generated number
 * @param max maximum generated number
 * @returns randomly generated integer
*/
int rand_int(const int min, const int max);


/**
 * Calculates distance between two points in a plane using Pythagorean theorem.
 * @returns square root of point1 - point2 squared
*/
float distance(const int point1, const int point2);


/**
 * Decides whether given target points are within the distance of radius.
 * @param row 'y' coordinate of target point
 * @param col 'x' coordinate of target point
 * @param center_row 'y' coordinate of circle center
 * @param center_col 'x' coordinate of circle center
 * @returns true if target points is inside circle, false otherwise
*/
bool inside_circle(const int row, const int col, const int center_row, const int center_col, const float radius);


/**
 * Displays circle at the given position.
 * @param row 'y' coordinate of circle center
 * @param col 'x' coordinate of circle center
 * @param radius radius of a circle
 * @param color_pair number of color pair to use (must be initialized beforehand)
*/
void render_circle(const int row, const int col, const float radius, const int color_pair);


/**
 * Displays text with number at the given position.
 * @param row 'y' coordinate of text on the screen
 * @param col 'x' coordinate of text on the screen
 * @param format text containing format
 * @param num number to be used with the format
 * @param color_pair number of color pair to use (must be initialized beforehand)
 */

void render_number(const int row, const int col, char *format, const int num, const int color_pair);


/**
 * Displays text at the given position.
 * @param row 'y' coordinate of text on the screen
 * @param col 'x' coordinate of text on the screen
 * @param text text to be displayed
 * @param color_pair number of color pair to use (must be initialized beforehand)
 */

void render_text(const int row, const int col, char *text, const int color_pair);


/**
 * Displays string at the given position.
 * @param row 'y' coordinate of text on the screen
 * @param col 'x' coordinate of text on the screen
 * @param format text containing format
 * @param string string to be used with the format
 * @param color_pair number of color pair to use (must be initialized beforehand)
*/
void render_string(const int row, const int col, char *format, char *string, const int color_pair);


/**
 *
 * Calculates radius using the entity's size.
 * @param size size of entity
 */
float get_radius(const int size);


// WINDOW *init_screen(void);
/**
 * Helper function to initialize basic curses window settings.
*/
void init_screen(void);


/**
 * Helper function to initialize curses color pairs.
*/
void init_colors(void);


/**
 * Detects other entities inside the given entity.
 * @attention Blobs do not count as an entity.
 * @attention It is implicit that given row & col +- radius inside the world bounds.
 * @attention This function doesn't take into account possible differences in radius, therefore it is only suitable on world spawn.
 * @param row 'y' coordinate of entity
 * @param col 'x' coordinate of entity
 * @param radius size of entity
 * @param size size of world
 * @param world map of a world containing entity indexes
 * @returns true if there is any entity within given radius, false otherwise
 */
bool check_collision(const int row, const int col, const int radius, const int size, int world[size][size]);


/**
 * Spawns entities on random coordinates in the world.
 * @implements check_collision() to prevent spawning inside one another.
 * @param row pointer to store randomly generated row
 * @param col pointer to store randomly generated col
 * @param size size of world
 * @param world map of a world containing entity indexes - used for collision detection
 * @returns randomly generated radius of spawned entity after successful generation, 0 otherwise
 */
float entity_spawn(int *row, int *col, const int size, int world[size][size]);


/**
 * Tries to randomly generate blob inside world.
 * Functions prevents spawning blob next to another blob.
 * @param blobs pointer to an amount of blobs already spawned
 * @param max_blobs maximum amount of blobs allowed to be spawned at the same time
 * @param size size of the world
 * @param world map of a world
*/
void blob_spawn(int *blobs, const int max_blobs, const int size, int world[size][size]);


/**
 * Bot vectors calculated according to player position & player size.
 * Bigger bots will generally more often head towards player while smaller ones will try to run away.
 * @attention To make game more interesting some degree of randomness is integrated in calculations.
 * @param n number of entities
 * @param params number of entity parameters
 * @param ent array of all entities (player & bots)
 * @param difficulty bot difficulty level
 */
void update_bot_vectors(const int n, const int params, int ent[n][params], const float difficulty);


/**
 * Updates player movement according to inputs.
 * Supports 2 input types: mouse (8 directions) / arrow keys (4 directions)
 * Major inconveniency in curses is that in order to get mouse cursor, we need mouse event, therefore click is needed.
 * @attention Mouse mask needs to be intialized in order to retrieve mouse events.
 * @param ch input character
 * @param row_vector pointer to row vector for updating
 * @param col_vector pointer to col vector for updating
 */
void update_player_vectors(const int ch, int *row_vector, int *col_vector);


/**
 * Updates positions of entities in both world & entities registry.
 * Moves entities in direction according to their vectors.
 * @param n number of entities
 * @param params number of entity parameters
 * @param ent array of all entities (player & bots)
 * @param size size of the world
 * @param world map of a world containing entity indexes
 *
 */
void update_positions(const int n, const int params, int ent[n][params], const int size, int world[size][size]);


/**
 * Performs collision evaluation with other entities & blobs.
 * @param n number of entities
 * @param params number of entity parameters
 * @param ent array of all entities (player & bots)
 * @param size size of the world
 * @param world map of a world containing entity indexes
 * @param blobs pointer to update blobs counter if entity picked up a blob
 * @param alive pointer to entities alive for updating
 */
void eval_positions(const int n, const int params, int ent[n][params], const int size, int world[size][size], int *blobs, int *alive);


/**
 * Displays menu with given heading and 2 options.
 * There are 2 options, user selects option by pressing SUBMIT_KEY.
 * @attention function assumes that noecho() is already enabled in stdscr init
 * @implements str_len() function
 * @param head heading to be used at the top of the menu
 * @param opt1 option1 text content
 * @param opt2 option2 text content
 * @returns true when user selects option1 || exits menu with MENU_KEY, false on option2
 */
bool heading_menu(char *head, char *option1, char *option2);


/**
 * Displays menu with heading and 3 options.
 * There are 3 options, user selects option by pressing SUBMIT_KEY
 * @attention function assumes that noecho() is already enabled in stdscr init
 * @implements str_len() function
 * @param head heading to be used at the top of the menu
 * @param opt1 option1 text content
 * @param opt2 option2 text content
 * @param opt3 option3 text content
 * @returns 0 if user exited menu with MENU_KEY, otherwise number of selected option 1-3
 */
int menu(char *head, char *opt1, char *opt2, char *opt3);


/**
 * Displays menu with label for user input.
 * User confirms input with KEY_ENTER.
 * @implements str_len() function
 * @param label label text content
 * @param len maximum length of input
 * @param input char array to store input
*/
void input_menu(char *label, const int input_len, char input[input_len]);


/**
 * Renders part of the world, filling entire viewport.
 * Player is always in the centre of world.
 * @attention That means the world could be rendered even behind bounds when player is near the edge.
 * @param n number of entities
 * @param params number of entity parameters
 * @param ent array of all entities (player & bots)
 * @param len size of names buffer
 * @param ent_names array containing entity names
 * @param size size of the world
 * @param world map of a world containing entity indexes
 * @param bots bots alive for displaying on screen
 */
void render_viewport(const int n, const int params, int ent[n][params], const int len, char ent_names[n][len], const int size, int world[size][size], const int bots);


/**
 * Starts interactive agar.io game
 * @param world_size dimensions of the generated world - usually larger than viewport
 * @param max_bots number of bots to generate inside the world
*/
void agario(const int world_size, const int max_bots);


