#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEAD                0
#define ALIVE               1

#define FPS_INTERVAL        1.0

#define CELL_SIZE           4 
#define PATTERN_MAX_SIZE    200

typedef struct param_list_s {

    char *filename;

    int height;
    int width;
    int period;

    uint64_t nb_max_gen;

    bool pause;
    bool debug;

} param_list_t;

inline static void print_help() {

    printf("Usage: life [option] \n");
    printf("Options: \n");
    printf("    -h        [Nb of row] \n");
    printf("    -w        [Nb of column] \n");
    printf("    -max_gen  [Maximum number of generation] \n");
    printf("    -period   [Time between 2 generations (in ms)] \n");
    printf("    -file     [Shape file (must be in shape folder)] \n");
    printf("    -debug    : Debug mode \n");
    printf("    -h        : Display this information \n");
    printf("\n\n");
    printf("Shape file synthax : \n");
    printf("    0 : Dead cell \n");
    printf("    1 : Alive cell \n");
    printf("    2 : End of row \n");
    printf("    3 : End of column (just need 1) \n");
    printf("\n");
    printf("Example with a square: \n");
    printf("    1 1 2 \n");
    printf("    1 1 2 \n");
    printf("    3         \n");
    printf("\n\n");

    exit(0);
}


inline static void init_param_list(param_list_t *p) {

    p->height           = 16;
    p->width            = 16;
    p->nb_max_gen       = 1000000L;
    p->period           = 250;
    p->pause            = false;
    p->debug            = false;
    p->filename         = "shape.txt";
}

inline static void print_param_list(param_list_t p) {

    printf("Params: \n");
    printf("    Height         : %d  \n", p.height);
    printf("    Width          : %d  \n", p.width);
    printf("    Nb max gen     : %lu \n", p.nb_max_gen);
    printf("    Period         : %d  \n", p.period);
    printf("    Debug          : %s  \n", p.debug ? "True" : "False");
    printf("\n");

}

inline static void read_param_cmd(int argc, char *argv[], param_list_t *p) {

    for (int i=1; i<argc; i+=2) {
        if (strcmp(argv[i], "-h") == 0 && argc > i + 1)
            p->height = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-w") == 0 && argc > i + 1)
            p->width = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-max_gen") == 0 && argc > i + 1)
            p->nb_max_gen = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-period") == 0 && argc > i + 1)
            p->period = atoi(argv[i+1]);
        else if (strcmp(argv[i], "-debug") == 0)
            p->debug = true;
        else if (strcmp(argv[i], "-file") == 0 && argc > i + 1)
            p->filename = argv[i+1];
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            print_help();
    } 

}

#endif /* GLOBAL_H_ */
