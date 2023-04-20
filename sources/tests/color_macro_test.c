#include <curses.h>
#include <stdio.h>

#define COLOR_ON(x) attron(COLOR_PAIR(x))
#define COLOR_OFF(x) attroff(COLOR_PAIR(x))

int main(void)
{
    initscr();

    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);

    CLR_ON(1);
    mvprintw(1, 1, "this is red\n");
    CLR_OFF(1);

    mvprintw(2, 1, "this is default\n");

    refresh();



    getchar();
    endwin();

    return 0;
}