#include <curses.h>
#include <math.h>

float distance(const int point1, const int point2)
{
    return sqrtf(point1 * point1 + point2 * point2);
}

bool inside_circle(const int row, const int col, const int center_row, const int center_col, const float radius)
{
    return distance(center_row - row, center_col - col) <= radius;
}

int draw_circle(const int row, const int col, float radius, const int color_pair)
{
    radius += 0.5;

    for (int i = row - radius; i <= row + radius; i++)
        for (int ii = col - radius; ii <= col + radius; ii++)
            if (inside_circle(i, ii, row, col, radius))
                // if (mvprintw(-2, -3, " ") == ERR)
                //     return ERR;
                mvprintw(i, ii, " ");

    return OK;
}

int draw_circumference(const int row, const int col, float radius, const int color_pair)
{
    radius += 0.5;
    int modiff = 10;

    for (int i = row - radius-modiff; i <= row + radius+modiff; i++)
        for (int ii = col - radius-modiff; ii <= col + radius+modiff; ii++) {
            float d = distance(row - i, col - ii);
            // if (distance(row - i, col - ii) >= radius-1 && distance(row - i, col - ii) <= radius)
            //     mvprintw(i, ii, " ");
            if (d >= radius && d <= radius+1)
                mvprintw(i, ii, " ");
        }

    return OK;
}

int main(void)
{
    initscr();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);

    attron(COLOR_PAIR(1));
    draw_circle(LINES/2, COLS/2, 7, 1);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    draw_circumference(LINES/2, COLS/2-20, 7, 1);
    //   draw_circumference(LINES/2, COLS/2, 4, 1);
    attroff(COLOR_PAIR(2));

    refresh();
    getchar();
    endwin();
}