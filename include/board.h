#ifndef BOARD_H_
#define BOARD_H_

#include <global.h>

/* Prototype */
int**   alloc_board(int height, int width);
int**   copy_board(int **board, int height, int width);
void    init_board(int **board, int height, int width, char *filename);
void    next_generation(int **board, int height, int width);
void    show_board(int **board, int height, int width); 

#endif /* BOARD_H_ */
