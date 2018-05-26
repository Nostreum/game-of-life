#include <board.h>
#include <file.h>
#include <emmintrin.h>
#include <simd.h>

int** alloc_board(int height, int width) {

    int **board = (int**) malloc(height * sizeof(int*));

    for (int i=0; i<height; i++) {
        board[i] = (int*) malloc(width * sizeof(int));

        for (int j=0; j<width; j++){
            board[i][j] = DEAD;
        }
    }

    return board;
}

int **copy_board(int **board, int height, int width) {

    int **board_c = alloc_board(height, width);

    for (int i=0; i<height; i++)
        for (int j=0; j<width; j++)
            board_c[i][j] = board[i][j];

    return board_c;
}

void init_board(int **board, int height, int width, char *filename) {
  
    int h, w;
    int shape[PATTERN_MAX_SIZE][PATTERN_MAX_SIZE];

    read_file(filename, &h, &w, shape);  
 
    for (int i = 0; i<h; i++) {
        for (int j=0; j<w; j++) {
            board[height/2 + j][width/2 + i] = shape[i][j];
        }
    }
}

void clean_board(int **board, int height, int width) {

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            board[i][j] = DEAD;
        }
    }

}

void next_generation(int **board, int height, int width) {

    int **board_c = copy_board(board, height, width);

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {

            if (i==0 || j==0 || i==height-1 || j==width-1) continue;

            int cs = board_c[i][j];

            int sum_alive = 0;
            for (int k=i-1; k<=i+1; k++) {
                for (int l=j-1; l<=j+1; l++) {
                    if (k == i && l == j) continue;
          
                    if (board_c[k][l] == ALIVE)  sum_alive++;
                }
            }

            // Cell is alive
            if (cs == ALIVE) {
                // Under or over population
                if (sum_alive < 2 || sum_alive > 3)
                board[i][j] = DEAD;
            }
            else if (cs == DEAD) {
                if (sum_alive == 3)
                    board[i][j] = ALIVE;
            }
            else{
                printf("Cell (%d, %d) has a wrong state (%d). Quiting. \n", i, j, cs);
                exit(0); 
            }
        }
    }

    for (int i=0; i<height; i++)
        free(board_c[i]);

    free(board_c);
}

// TODO: In progress
//void next_generation_simd(int_a **board, int height, int width) {
//
//    int_a **board_c = copy_board(board, height, width);
//
//    vint32 a0, a1, a2;
//    vint32 b0, b1, b2;
//    vint32 c0, c1, c2;
//   
//    vint32 l0, l1, l2;
//    vint32 r0, r1, r2;
//    vint32 s0, s1, s2;
//
//    vint32 m0 = vint32_set(0, 0xFFFF, 0, 0);
//    vint32 m1 = vint32_set(0, 0, 0xFFFF, 0);
//    vint32 mask_j = vint32_set(0xFFFF, 0xFFFF, 0xFFFF, 0);
// 
//    for (int i=0; i<height; i++) {
//        
//        a0 = vint32_load2D(board, 0, 0);
//        a1 = vint32_load2D(board, 1, 0);
//        a2 = vint32_load2D(board, 2, 0);
//
//        b0 = vint32_load2D(board, 0, 1);
//        b1 = vint32_load2D(board, 1, 1);
//        b2 = vint32_load2D(board, 2, 1);
//
//        for (int j=0; j<width/4; j++) {
//
//            /* Load new column */
//            c0 = vint32_load2D(board, i-1, j+1);
//            c1 = vint32_load2D(board, i,   j+1);
//            c2 = vint32_load2D(board, i+1, j+1);
//
//            /* left shift */
//            l0 = vint32_left(a0, b0);
//            l1 = vint32_left(a1, b1);
//            l2 = vint32_left(a2, b2);
//
//            /* right shift */
//            r0 = vint32_right(b0, c0);
//            r1 = vint32_right(b1, c1);
//            r2 = vint32_right(b2, c2);
//
//            /* Compute 1st point */
//            s0 = vint32_add3(l0, l1, l2);
//            s0 = vint32_sub(s0, vint32_and(b1, m0));
//            s0 = vint32_and(s0, mask_j);
//            s0 = vint32_hadd(s0);
//            s0 = vint32_hadd(s0);
//
//            /* Compute 2nd point */
//
//            /* Compute 3th point */
//
//            /* Compute 4th point */
//
//            /* Register rotation */
//            a0 = b0;
//            a1 = b1;
//            a2 = b2;
//
//            b0 = c0;
//            b1 = c1;
//            b2 = c2;
//            
//        }
//    }
//
//}

void update_board(int **board, int x, int y, int new_state) {
    board[x][y] = new_state;
}

void show_board(int **board, int height, int width) {

    for (int i=0; i<height; i++) {
        printf("|");
        for (int j=0; j<width; j++) {
            printf(" %d |", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

