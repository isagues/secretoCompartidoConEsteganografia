#include <stdint.h>
#include <stdbool.h>

typedef enum Action {
    DISTRIBUTE  = 'd',
    RECOVER     = 'r',
} Action;

typedef enum LoggingLevel {
    INFO,
    VERBOSE,
    DEBUG,
} LoggingLevel;

typedef struct Arguments {
    Action          action;
    uint8_t         k;
    char *          secretImage;
    char *          shadowsDir;
    char *          shadesOutputDir;
    LoggingLevel    loggingLevel;
} Arguments;

Arguments args_parse_and_validate(int argc, char *argv[]);
