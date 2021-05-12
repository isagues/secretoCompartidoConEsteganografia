#include <stdint.h>

enum Action{DISTRIBUTE='d', RECOVER='r'};

typedef struct Arguments {

    enum Action action;
    uint8_t     k;
    char *      secretImage;
    char *      shadowsDir;
} Arguments;

Arguments args_parse_and_validate(int argc, char *argv[]);