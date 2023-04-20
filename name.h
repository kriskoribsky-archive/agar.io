// LIBRARY "names.h"
// AUTHOR: KRISTIAN KORIBSKY
// DATE: 11.12.2022
// ==========================================================================
#define NAMELIST_FILENAME "names.txt"


/**
 * Generates random file line number.
 * @attention Uses rand() function so srand() initialization is needed!
 * @param line pointer to save generated line number
 * @param length pointer to save length of the longest word in file
 * @returns if the file cannot be read 1, otherwise if everything ok, 0 is returned
 * 
*/
int random_line(int *line, int *longest_word);


/**
 * Gets random word on specified line.
 * @attention Uses rand() function so srand() initialization is needed!
 * @param line line number to read
 * @param max_l max length of buffer
 * @param name buffer where random name will be put
 * @returns if the file cannot be read 1, otherwise if everything ok, 0 is returned
*/
int get_line(const int line, const int len, char name[]);