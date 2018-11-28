#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmd_helper.h"

void handle_cmd_arguments(int argc, char *argv[], char **meta_data_file, int *count_of_generations, int *mutation_percentage) {

    // checks if arguments are valid
    if (argc < 3) {
        printf("Error: Expected at least 2 arguments! Actual count of arguments: %d", argc-1); // prvni argument je soubor spusteni aplikace, proto (argc-1)
        return;
    }

    // handle arguments
    *meta_data_file       = argv[1];
    *count_of_generations = atoi(argv[2]);

    if (argv[3] == "-m") {
        *mutation_percentage = atoi(argv[4]);
    } else {
        *mutation_percentage = 5; //defaultne 5%
    }
}
