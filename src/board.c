#include <board.h>
#include <file.h>
#include <debug.h>

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

int_a **copy_board_a(int_a **board, int height, int width) {

    int_a **board_c = alloc_board(height, width);

    for (int i=0; i<height; i++)
        for (int j=0; j<width; j++)
            board_c[i][j] = board[i][j];

    return board_c;
}

void init_board(int **board, int height, int width, char *filename) {
  
    int h, w;
    int shape[PATTERN_MAX_SIZE][PATTERN_MAX_SIZE];

    read_file(filename, &h, &w, shape);  
 
    for (int i = 0; i<w; i++) {
        for (int j=0; j<h; j++) {
            board[height/2 + j][width/2 + i] = shape[j][i];
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

            if (cs == ALIVE) {
                // Under or over population
                if (sum_alive < 2 || sum_alive > 3)
                board[i][j] = DEAD;
            }
            else if (cs == DEAD) {
                // Born
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

void next_generation_simd_i32(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    vint32 a0, a1, a2;
    vint32 b0, b1, b2;
    vint32 c0, c1, c2;

    vint32 l0, l1, l2;
    vint32 r0, r1, r2;

    vint32 la, ra;
    vint32 sla1, sla2;
    vint32 sra1, sra2;

    vint32 final;

    vint32 z = vint32_setall(0);
    vint32 one = vint32_setall(0);
    vint32 two = vint32_setall(2);
    vint32 three = vint32_setall(3);

    vint32 lt, gt, eq, lt_o_gt, updt;

    // TODO: Just right shift
    vint32 mask0 = vint32_set(0xFFFF, 0, 0, 0);
    vint32 mask1 = vint32_set(0, 0xFFFF, 0, 0);
    vint32 mask2 = vint32_set(0, 0, 0xFFFF, 0);
    vint32 mask3 = vint32_set(0, 0, 0, 0xFFFF);

    for (int i=1; i<height-1; i++) {
       
        a0 = vint32_load2D(board_c, i-1, 0);
        a1 = vint32_load2D(board_c, i,   0);
        a2 = vint32_load2D(board_c, i+1, 0);

        b0 = vint32_load2D(board_c, i-1, 1);
        b1 = vint32_load2D(board_c, i,   1);
        b2 = vint32_load2D(board_c, i+1, 1);

        for (int j=1; j<width/4-1; j++) {
  
            /* Load new column */
            c0 = vint32_load2D(board_c, i-1, j+1);
            c1 = vint32_load2D(board_c, i,   j+1);
            c2 = vint32_load2D(board_c, i+1, j+1);

            /* left shift */
            l0 = vint32_left(a0, b0);
            l1 = vint32_left(a1, b1);
            l2 = vint32_left(a2, b2);

            /* right shift */
            r0 = vint32_right(b0, c0);
            r1 = vint32_right(b1, c1);
            r2 = vint32_right(b2, c2);       

            /* Sum */
            la = vint32_add3(l0, l1, l2);
            ra = vint32_add3(r0, r1, r2);

            /* Compute */
            sla1 = vint32_and(vint32_haddf(vint32_left2(z, la)), mask2);
            sla2 = vint32_and(vint32_haddf(vint32_right2(la, z)),mask3);

            sra1 = vint32_and(vint32_haddf(vint32_left2(z, ra)), mask0);
            sra2 = vint32_and(vint32_haddf(vint32_right2(ra, z)), mask1);

            final = vint32_add4(sla1, sla2, sra1, sra2);
            final = vint32_sub(final, b1);
           
            /* Rules */
            lt = vint32_lt(final, two);
            gt = vint32_gt(final, three);
            eq = vint32_eq(final, three);

            lt_o_gt = vint32_or(lt, gt);

            updt = vint32_or(vint32_and(lt_o_gt, b1), vint32_and(eq, vint32_not32(b1)));
           
            final = vint32_add(updt, b1);
            final = vint32_and(final, vint32_lt(final, two));

            /* Storing results */
            vint32_store2D(board, i, j, final);

            /* Register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }
}

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

