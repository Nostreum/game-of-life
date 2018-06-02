#ifndef BOARD_H_
#define BOARD_H_

#include <global.h>
#include <simd.h>

/* Prototype */
int**   alloc_board(int height, int width);
int**   copy_board(int **board, int height, int width);
void    init_board(int **board, int height, int width, char *filename);
void    show_board(int **board, int height, int width); 
void    update_board(int **board, int x, int y, int new_state);
void    clean_board(int **board, int height, int width);

void    next_generation(int **board, int height, int width);
void    next_generation_openmp(int **board, int height, int width);
void    next_generation_simd_i32(int_a **board, int height, int width);
void    next_generation_simd_i32_openmp(int_a **board, int height, int width);
void    next_generation_simd_i16(int_a **board, int height, int width);
void    next_generation_simd_i16_openmp(int_a **board, int height, int width);
void    next_generation_simd_i8(int_a **board, int height, int width);
void    next_generation_simd_i8_openmp(int_a **board, int height, int width);

#endif /* BOARD_H_ */
