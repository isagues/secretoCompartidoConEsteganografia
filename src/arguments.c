#include "arguments.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_COUNT  5 // 1 + 4
#define HELP_STRING \
        "ss d|r secret_image_relative_path k_value shadows_dir_path [shades_output_dir] [-v|-d]\n\n \
        - shades_output_dir only available on distribute mode.\n \
        -v: verbose. Logs about program status.\n \
        -d: debug. Detailed error logs.\n"

Arguments args_parse_and_validate(int argc, char *argv[]) {
    unsigned argCount = (unsigned) argc;


    if(strcmp(argv[1], "help") == 0) {
        printf(HELP_STRING);
        exit(0);
    }

    if (argc < ARG_COUNT) {
        fprintf(stderr, "Argument count missmatch. Make sure to call the program like:\n");
        fprintf(stderr, HELP_STRING);
        exit(1);
    }

    Arguments args;
    args.loggingLevel = INFO;
    args.shadesOutputDir = NULL;

    args.action = argv[1][0];

    if (args.action != DISTRIBUTE && args.action != RECOVER) {
        fprintf(stderr, "Invalid first param. Available options are [%c] for distributing the secret and [%c] to recover it.", DISTRIBUTE, RECOVER);
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

    // Extra params
    for(size_t i = 5; i < argCount; i++) {
        if(argv[i][0] != '-' && args.shadesOutputDir == NULL && args.action == DISTRIBUTE) {
            args.shadesOutputDir = argv[i];
        }
        else if(args.loggingLevel == INFO) {
            if(strcmp(argv[i], "-v") == 0) {
                if(args.loggingLevel == INFO) {
                    args.loggingLevel = VERBOSE;
                }
            }
            else if(strcmp(argv[i], "-d") == 0) {
                if(args.loggingLevel == INFO) {
                    args.loggingLevel = DEBUG;
                }
            }
            else {
                fprintf(stderr, "Invalid extra param %s. It will be ignored.\n", argv[i]);
            }
        }
        else {
            fprintf(stderr, "Invalid extra param %s. It will be ignored.\n", argv[i]);
        }
    }

    if(args.shadesOutputDir == NULL) {
        // Default
        args.shadesOutputDir = "images";
    }

    return args;
}