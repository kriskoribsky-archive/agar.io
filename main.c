#include "agario.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // init random seed
    srand(time(NULL));

    // command line argument input
    if (argc != GAME_PARAMETERS+1) {
        printf("Wrong number of arguments!\nUsage: ./agar.io <WORLD-SIZE> <NUMBER-OF-BOTS>\n");
        return EXIT_FAILURE;
    }

    // parse command line arguments
    int world_size, bot_count;
    sscanf(argv[1], "%d", &world_size);
    sscanf(argv[2], "%d", &bot_count);

    // input checking
    if (world_size < MIN_WORLD_SIZE || world_size > MAX_WORLD_SIZE) {
        printf("Incorrect size of the world. Should be in range %d to %d.\n", MIN_WORLD_SIZE, MAX_WORLD_SIZE);
        return EXIT_FAILURE;
    }
    if (bot_count < MIN_BOT_COUNT || bot_count > MAX_BOT_COUNT) {
        printf("Incorrect number of bots. Should be in range %d to %d.\n", MIN_BOT_COUNT, MAX_BOT_COUNT);
        return EXIT_FAILURE;
    }

    agario(world_size, bot_count);

    return EXIT_SUCCESS;
}