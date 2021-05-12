#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>

#define ARG_COUNT  5 // 1 + 4

Arguments args_parse_and_validate(int argc, char *argv[]) {

    if (argc != ARG_COUNT) {
        fprintf(stderr, "Argument count missmatch. Make sure to call the program like:\n \
        ss [d|r] [Secret image path] [k value] [Shadows directory path].\n");
        exit(1);
    }

    Arguments args;

    args.action = argv[1][0];

    if (args.action != 'd' && args.action != 'r') {
        fprintf(stderr, "Invalid first param. Available options are [d] for distributing the secret and [r] to recover it.");
        exit(1);
    }

    args.secretImage = argv[2];
    
    int tmpK = atoi(argv[3]);

    if(tmpK > UINT8_MAX) {
        fprintf(stderr, "Invalid third param. K value should be lower or equal than %d.", UINT8_MAX);
        exit(1);
    }

    args.k = tmpK;

    args.shadowsDir = argv[4];

    return args;
}