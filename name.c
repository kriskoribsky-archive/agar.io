// IMPLEMENTATION of library "names.h"
// AUTHOR: KRISTIAN KORIBSKY
// DATE: 11.12.2022
// ==========================================================================
#include "name.h"
#include <stdio.h>
#include <stdlib.h>


int random_line(int *line, int *longest_word)
{
    FILE *fp = fopen(NAMELIST_FILENAME, "r");

    if(fp == NULL)
        return 1;

    int ch;
    int lc = 1, max_l = 0, l = 0;

    // count lines in file
    while ((ch = getc(fp)) != EOF) {
        l++;

        if (ch == '\n') {
            lc++;

            if (l > max_l)
                max_l = l;
            
            l = 0;
        }
    }

    *line = rand() % (lc + 1);
    *longest_word = max_l + 1;        // +1 for null terminator

    fclose(fp);
    return 0;
}


int get_line(const int line, const int max_l, char name[])
{
    FILE *fp = fopen(NAMELIST_FILENAME, "r");

    if(fp == NULL)
        return 1;

    int ch;
    int lc = 0;
    while ((ch = getc(fp)) != EOF) {
        if (ch == '\n')
            lc++;
        
        if (lc == line) {
            int line_ch, line_i;
            for (line_i=0; (line_ch = getc(fp)) != '\n' && line_i < max_l; line_i++)
                name[line_i] = line_ch;

            name[line_i] = '\0';     // terminate string at the end
            break;
        }
    }
    fclose(fp);
    return 0;
}