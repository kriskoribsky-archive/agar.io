#define _POSIX_C_SOURCE 200201L
#include <time.h>
#include <stdio.h>

void sleep_ms(const int milliseconds)
{
    struct timespec ts = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000};
    nanosleep(&ts, NULL);
}

int main(void)
{
    int counter;
    while (1) {
        if (counter % 3 == 0)
            printf("%d: Function called every 6 secs executed.\n", counter);

        printf("%d: Function called every 2 secs executed.\n", counter);
        counter += 1;
        sleep_ms(2000);
    }
}