#ifndef __ARGS_H__
#define __ARGS_H__

#include "log/log.h"
#include "galois/galois.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum Action {
    DISTRIBUTE  = 'd',
    RECOVER     = 'r',
} Action;

typedef struct Args {
    Action          action;
    uint8_t         k;
    char *          secretImage;
    char *          shadowsDir;
    char *          shadesOutputDir;
    bool            padding;
    galois2_8_gen_t galoisGen;
    enum LogLevel   loglevel;
    bool            logQuiet;
    bool            logVerbose;
} Args;

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecución.
 */
bool args_parse(const int argc, char **argv, Args *args);

#endif