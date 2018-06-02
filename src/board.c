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

void free_board(int **board, int height, int width) {

    for(int i=0; i<height; i++)
        free(board[i]);

    free(board);
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

    free_board(board_c, height, width);
}

void next_generation_openmp(int **board, int height, int width) {

    int **board_c = copy_board(board, height, width);

    #pragma omp parallel for default(none) firstprivate(height, width) shared(board_c, board)
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

    free_board(board_c, height, width);
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
    vint32 two = vint32_setall(2);
    vint32 three = vint32_setall(3);

    vint32 lt, gt, eq, lt_o_gt, updt;

    // todo: just right shift
    vint32 mask0 = vint32_set(0xffff, 0, 0, 0);
    vint32 mask1 = vint32_set(0, 0xffff, 0, 0);
    vint32 mask2 = vint32_set(0, 0, 0xffff, 0);
    vint32 mask3 = vint32_set(0, 0, 0, 0xffff);

    for (int i = 1; i < height-1; i++) {
       
        a0 = vint32_load2D(board_c, i-1, 0);
        a1 = vint32_load2D(board_c, i,   0);
        a2 = vint32_load2D(board_c, i+1, 0);

        b0 = vint32_load2D(board_c, i-1, 1);
        b1 = vint32_load2D(board_c, i,   1);
        b2 = vint32_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 4) - 1; j++) {
  
            /* load new column */
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

            /* sum */
            la = vint32_add3(l0, l1, l2);
            ra = vint32_add3(r0, r1, r2);

            /* compute */
            sra1 = vint32_and(vint32_haddf(vint32_left2(z, ra)), mask0);
            sra2 = vint32_and(vint32_haddf(vint32_right2(ra, z)), mask1);

            sla1 = vint32_and(vint32_haddf(vint32_left2(z, la)), mask2);
            sla2 = vint32_and(vint32_haddf(vint32_right2(la, z)),mask3);
            
            final = vint32_add4(sla1, sla2, sra1, sra2);
            final = vint32_sub(final, b1);
           
            /* rules */
            lt = vint32_lt(final, two);
            gt = vint32_gt(final, three);
            eq = vint32_eq(final, three);

            lt_o_gt = vint32_or(lt, gt);

            updt = vint32_or(vint32_and(lt_o_gt, b1), vint32_and(eq, vint32_not32(b1)));
           
            final = vint32_add(updt, b1);
            final = vint32_and(final, vint32_lt(final, two));

            /* storing results */
            vint32_store2D(board, i, j, final);

            /* register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }
    
    free_board(board_c, height, width);
}

void next_generation_simd_i32_openmp(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    
    vint32 z = vint32_setall(0);
    vint32 two = vint32_setall(2);
    vint32 three = vint32_setall(3);

    
    // todo: just right shift
    vint32 mask0 = vint32_set(0xffff, 0, 0, 0);
    vint32 mask1 = vint32_set(0, 0xffff, 0, 0);
    vint32 mask2 = vint32_set(0, 0, 0xffff, 0);
    vint32 mask3 = vint32_set(0, 0, 0, 0xffff);

    #pragma omp parallel for default(none) firstprivate(height, width) shared(board_c, board, z, two, three, mask0, mask1, mask2, mask3)
    for (int i = 1; i < height-1; i++) {
      
        vint32 a0, a1, a2;
        vint32 b0, b1, b2;
        vint32 c0, c1, c2;

        vint32 l0, l1, l2;
        vint32 r0, r1, r2;

        vint32 la, ra;
        vint32 sla1, sla2;
        vint32 sra1, sra2;

        vint32 final;

        vint32 lt, gt, eq, lt_o_gt, updt;

        a0 = vint32_load2D(board_c, i-1, 0);
        a1 = vint32_load2D(board_c, i,   0);
        a2 = vint32_load2D(board_c, i+1, 0);

        b0 = vint32_load2D(board_c, i-1, 1);
        b1 = vint32_load2D(board_c, i,   1);
        b2 = vint32_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 4) - 1; j++) {
  
            /* load new column */
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

            /* sum */
            la = vint32_add3(l0, l1, l2);
            ra = vint32_add3(r0, r1, r2);

            /* compute */
            sra1 = vint32_and(vint32_haddf(vint32_left2(z, ra)), mask0);
            sra2 = vint32_and(vint32_haddf(vint32_right2(ra, z)), mask1);

            sla1 = vint32_and(vint32_haddf(vint32_left2(z, la)), mask2);
            sla2 = vint32_and(vint32_haddf(vint32_right2(la, z)),mask3);
            
            final = vint32_add4(sla1, sla2, sra1, sra2);
            final = vint32_sub(final, b1);
           
            /* rules */
            lt = vint32_lt(final, two);
            gt = vint32_gt(final, three);
            eq = vint32_eq(final, three);

            lt_o_gt = vint32_or(lt, gt);

            updt = vint32_or(vint32_and(lt_o_gt, b1), vint32_and(eq, vint32_not32(b1)));
           
            final = vint32_add(updt, b1);
            final = vint32_and(final, vint32_lt(final, two));

            /* storing results */
            vint32_store2D(board, i, j, final);

            /* register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }

    free_board(board_c, height, width);
}

//TODO: Not totally functional
void next_generation_simd_i16(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    vint16 a0, a1, a2;
    vint16 b0, b1, b2;
    vint16 c0, c1, c2;

    vint16 l0, l1, l2;
    vint16 r0, r1, r2;

    vint16 la, ra;
    vint16 sla1, sla2, sla3, sla4;
    vint16 sra1, sra2, sra3, sra4;

    vint16 final;

    vint16 two = vint16_setall(2);
    vint16 three = vint16_setall(3);

    vint16 lt, gt, eq, lt_o_gt, updt;

    vint16 masksuml  = vint16_set(0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0);
    vint16 masksumr  = vint16_set(0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF);
    vint16 mask      = vint16_set(0xFF, 0, 0, 0, 0, 0, 0, 0);
    
    for (int i = 1; i < height-1; i++) {
       
        a0 = vint16_load2D(board_c, i-1, 0);
        a1 = vint16_load2D(board_c, i,   0);
        a2 = vint16_load2D(board_c, i+1, 0);

        b0 = vint16_load2D(board_c, i-1, 1);
        b1 = vint16_load2D(board_c, i,   1);
        b2 = vint16_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 8) - 1; j++) {
  
            /* Load new column */
            c0 = vint16_load2D(board_c, i-1, j+1);
            c1 = vint16_load2D(board_c, i,   j+1);
            c2 = vint16_load2D(board_c, i+1, j+1);

            /* left shift */
            l0 = vint16_left(a0, b0);
            l1 = vint16_left(a1, b1);
            l2 = vint16_left(a2, b2);

            /* right shift */
            r0 = vint16_right(b0, c0);
            r1 = vint16_right(b1, c1);
            r2 = vint16_right(b2, c2);       

            /* Sum */
            la = vint16_add3(l0, l1, l2);
            ra = vint16_add3(r0, r1, r2);

            /* Compute */
            sra1 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 0), ra)), mask);
            sra2 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 1), ra)), _mm_srli_epi16(mask, 1)); 
            sra3 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 2), ra)), _mm_srli_epi16(mask, 2));
            sra4 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 3), ra)), _mm_srli_epi16(mask, 3));
            
            sla1 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 0), la)), _mm_srli_epi16(mask, 4));
            sla2 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 1), la)), _mm_srli_epi16(mask, 5));
            sla3 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 2), la)), _mm_srli_epi16(mask, 6));
            sla4 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 3), la)), _mm_srli_epi16(mask, 7));

            final = vint16_add8(sla1, sla2, sla3, sla4, sra1, sra2, sra3, sra4);
            final = vint16_sub(final, b1);
           
            /* Rules */
            lt = vint16_lt(final, two);
            gt = vint16_gt(final, three);
            eq = vint16_eq(final, three);

            lt_o_gt = vint16_or(lt, gt);

            updt = vint16_or(vint16_and(lt_o_gt, b1), vint16_and(eq, vint16_not16(b1)));
           
            final = vint16_add(updt, b1);
            final = vint16_and(final, vint16_lt(final, two));

            /* Storing results */
            vint16_store2D(board, i, j, final);

            /* Register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }

    free_board(board_c, height, width);
}

//TODO: Not totally functional
void next_generation_simd_i16_openmp(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    
    vint16 two = vint16_setall(2);
    vint16 three = vint16_setall(3);

    vint16 masksuml  = vint16_set(0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0);
    vint16 masksumr  = vint16_set(0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF);
    vint16 mask      = vint16_set(0xFF, 0, 0, 0, 0, 0, 0, 0);
    
    #pragma omp parallel for default(none) firstprivate(height, width) shared(board_c, board, two, three, masksuml, masksumr, mask)
    for (int i = 1; i < height-1; i++) {
       
        vint16 a0, a1, a2;
        vint16 b0, b1, b2;
        vint16 c0, c1, c2;

        vint16 l0, l1, l2;
        vint16 r0, r1, r2;

        vint16 la, ra;
        vint16 sla1, sla2, sla3, sla4;
        vint16 sra1, sra2, sra3, sra4;

        vint16 final;
        vint16 lt, gt, eq, lt_o_gt, updt;
            
        a0 = vint16_load2D(board_c, i-1, 0);
        a1 = vint16_load2D(board_c, i,   0);
        a2 = vint16_load2D(board_c, i+1, 0);

        b0 = vint16_load2D(board_c, i-1, 1);
        b1 = vint16_load2D(board_c, i,   1);
        b2 = vint16_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 8) - 1; j++) {
  
            /* Load new column */
            c0 = vint16_load2D(board_c, i-1, j+1);
            c1 = vint16_load2D(board_c, i,   j+1);
            c2 = vint16_load2D(board_c, i+1, j+1);

            /* left shift */
            l0 = vint16_left(a0, b0);
            l1 = vint16_left(a1, b1);
            l2 = vint16_left(a2, b2);

            /* right shift */
            r0 = vint16_right(b0, c0);
            r1 = vint16_right(b1, c1);
            r2 = vint16_right(b2, c2);       

            /* Sum */
            la = vint16_add3(l0, l1, l2);
            ra = vint16_add3(r0, r1, r2);

            /* Compute */
            sra1 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 0), ra)), mask);
            sra2 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 1), ra)), _mm_srli_epi16(mask, 1)); 
            sra3 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 2), ra)), _mm_srli_epi16(mask, 2));
            sra4 = vint16_and(vint16_haddf(vint16_and(_mm_srli_epi16(masksuml, 3), ra)), _mm_srli_epi16(mask, 3));
            
            sla1 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 0), la)), _mm_srli_epi16(mask, 4));
            sla2 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 1), la)), _mm_srli_epi16(mask, 5));
            sla3 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 2), la)), _mm_srli_epi16(mask, 6));
            sla4 = vint16_and(vint16_haddf(vint16_and(_mm_slli_epi16(masksumr, 3), la)), _mm_srli_epi16(mask, 7));

            final = vint16_add8(sla1, sla2, sla3, sla4, sra1, sra2, sra3, sra4);
            final = vint16_sub(final, b1);
           
            /* Rules */
            lt = vint16_lt(final, two);
            gt = vint16_gt(final, three);
            eq = vint16_eq(final, three);

            lt_o_gt = vint16_or(lt, gt);

            updt = vint16_or(vint16_and(lt_o_gt, b1), vint16_and(eq, vint16_not16(b1)));
           
            final = vint16_add(updt, b1);
            final = vint16_and(final, vint16_lt(final, two));

            /* Storing results */
            vint16_store2D(board, i, j, final);

            /* Register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }

    free_board(board_c, height, width);
}

// TODO: Not totally functional
void next_generation_simd_i8(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    vint8 a0, a1, a2;
    vint8 b0, b1, b2;
    vint8 c0, c1, c2;

    vint8 l0, l1, l2;
    vint8 r0, r1, r2;

    vint8 la, ra;
    vint8 sla1, sla2, sla3, sla4, sla5, sla6, sla7, sla8;
    vint8 sra1, sra2, sra3, sra4, sra5, sra6, sra7, sra8;

    vint8 final;

    vint8 two = vint8_setall(2);
    vint8 three = vint8_setall(3);

    vint8 lt, gt, eq, lt_o_gt, updt;

    vint8 masksuml  = vint8_set(0xF, 0xF, 0xF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    vint8 masksumr  = vint8_set(0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0, 0xF, 0xF, 0xF);
    vint8 mask      = vint8_set(0xF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    for (int i = 1; i < height-1; i++) {
       
        a0 = vint8_load2D(board_c, i-1, 0);
        a1 = vint8_load2D(board_c, i,   0);
        a2 = vint8_load2D(board_c, i+1, 0);

        b0 = vint8_load2D(board_c, i-1, 1);
        b1 = vint8_load2D(board_c, i,   1);
        b2 = vint8_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 16) - 1; j++) {
  
            /* Load new column */
            c0 = vint8_load2D(board_c, i-1, j+1);
            c1 = vint8_load2D(board_c, i,   j+1);
            c2 = vint8_load2D(board_c, i+1, j+1);

            /* left shift */
            l0 = vint8_left(a0, b0);
            l1 = vint8_left(a1, b1);
            l2 = vint8_left(a2, b2);

            /* right shift */
            r0 = vint8_right(b0, c0);
            r1 = vint8_right(b1, c1);
            r2 = vint8_right(b2, c2);       

            /* Sum */
            la = vint8_add3(l0, l1, l2);
            ra = vint8_add3(r0, r1, r2);

            /* Compute */
            sra1 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 0), ra)), mask);
            sra2 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 1), ra)), _mm_srli_si128(mask, 1)); 
            sra3 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 2), ra)), _mm_srli_si128(mask, 2));
            sra4 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 4), ra)), _mm_srli_si128(mask, 3));
            sra5 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 5), ra)), _mm_srli_si128(mask, 4));
            sra6 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 6), ra)), _mm_srli_si128(mask, 5)); 
            sra7 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 7), ra)), _mm_srli_si128(mask, 6));
            sra8 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 8), ra)), _mm_srli_si128(mask, 7));

            sla1 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 0), la)), _mm_srli_si128(mask, 8));
            sla2 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 1), la)), _mm_srli_si128(mask, 9));
            sla3 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 2), la)), _mm_srli_si128(mask, 10));
            sla4 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 3), la)), _mm_srli_si128(mask, 11));
            sla5 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 4), la)), _mm_srli_si128(mask, 12));
            sla6 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 5), la)), _mm_srli_si128(mask, 13));
            sla7 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 6), la)), _mm_srli_si128(mask, 14));
            sla8 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 7), la)), _mm_srli_si128(mask, 15));

            final = vint8_add(vint8_add8(sla1, sla2, sla3, sla4, sla5, sla6, sla7, sla8), vint8_add8(sra1, sra2, sra3, sra4, sra5, sra6, sra7, sra8));
            final = vint8_sub(final, b1);
           
            /* Rules */
            lt = vint8_lt(final, two);
            gt = vint8_gt(final, three);
            eq = vint8_eq(final, three);

            lt_o_gt = vint8_or(lt, gt);

            updt = vint8_or(vint8_and(lt_o_gt, b1), vint8_and(eq, vint8_not8(b1)));
           
            final = vint8_add(updt, b1);
            final = vint8_and(final, vint8_lt(final, two));

            /* Storing results */
            vint8_store2D(board, i, j, final);

            /* Register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }

    free_board(board_c, height, width);
}

// TODO: Not totally functional
void next_generation_simd_i8_openmp(int_a **board, int height, int width) {

    int_a **board_c = copy_board_a(board, height, width);

    vint8 two = vint8_setall(2);
    vint8 three = vint8_setall(3);

    vint8 masksuml  = vint8_set(0xF, 0xF, 0xF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    vint8 masksumr  = vint8_set(0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0, 0xF, 0xF, 0xF);
    vint8 mask      = vint8_set(0xF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   
    #pragma omp parallel for default(none) firstprivate(height, width) shared(board_c, board, two, three, masksuml, masksumr, mask) 
    for (int i = 1; i < height-1; i++) {

        vint8 a0, a1, a2;
        vint8 b0, b1, b2;
        vint8 c0, c1, c2;

        vint8 l0, l1, l2;
        vint8 r0, r1, r2;

        vint8 la, ra;
        vint8 sla1, sla2, sla3, sla4, sla5, sla6, sla7, sla8;
        vint8 sra1, sra2, sra3, sra4, sra5, sra6, sra7, sra8;

        vint8 final;
        vint8 lt, gt, eq, lt_o_gt, updt;
        
        a0 = vint8_load2D(board_c, i-1, 0);
        a1 = vint8_load2D(board_c, i,   0);
        a2 = vint8_load2D(board_c, i+1, 0);

        b0 = vint8_load2D(board_c, i-1, 1);
        b1 = vint8_load2D(board_c, i,   1);
        b2 = vint8_load2D(board_c, i+1, 1);

        for (int j = 1; j < (width / 16) - 1; j++) {
  
            /* Load new column */
            c0 = vint8_load2D(board_c, i-1, j+1);
            c1 = vint8_load2D(board_c, i,   j+1);
            c2 = vint8_load2D(board_c, i+1, j+1);

            /* left shift */
            l0 = vint8_left(a0, b0);
            l1 = vint8_left(a1, b1);
            l2 = vint8_left(a2, b2);

            /* right shift */
            r0 = vint8_right(b0, c0);
            r1 = vint8_right(b1, c1);
            r2 = vint8_right(b2, c2);       

            /* Sum */
            la = vint8_add3(l0, l1, l2);
            ra = vint8_add3(r0, r1, r2);

            /* Compute */
            sra1 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 0), ra)), mask);
            sra2 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 1), ra)), _mm_srli_si128(mask, 1)); 
            sra3 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 2), ra)), _mm_srli_si128(mask, 2));
            sra4 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 4), ra)), _mm_srli_si128(mask, 3));
            sra5 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 5), ra)), _mm_srli_si128(mask, 4));
            sra6 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 6), ra)), _mm_srli_si128(mask, 5)); 
            sra7 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 7), ra)), _mm_srli_si128(mask, 6));
            sra8 = vint8_and(vint8_haddf(vint8_and(_mm_srli_si128(masksuml, 8), ra)), _mm_srli_si128(mask, 7));

            sla1 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 0), la)), _mm_srli_si128(mask, 8));
            sla2 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 1), la)), _mm_srli_si128(mask, 9));
            sla3 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 2), la)), _mm_srli_si128(mask, 10));
            sla4 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 3), la)), _mm_srli_si128(mask, 11));
            sla5 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 4), la)), _mm_srli_si128(mask, 12));
            sla6 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 5), la)), _mm_srli_si128(mask, 13));
            sla7 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 6), la)), _mm_srli_si128(mask, 14));
            sla8 = vint8_and(vint8_haddf(vint8_and(_mm_slli_si128(masksumr, 7), la)), _mm_srli_si128(mask, 15));

            final = vint8_add(vint8_add8(sla1, sla2, sla3, sla4, sla5, sla6, sla7, sla8), vint8_add8(sra1, sra2, sra3, sra4, sra5, sra6, sra7, sra8));
            final = vint8_sub(final, b1);
           
            /* Rules */
            lt = vint8_lt(final, two);
            gt = vint8_gt(final, three);
            eq = vint8_eq(final, three);

            lt_o_gt = vint8_or(lt, gt);

            updt = vint8_or(vint8_and(lt_o_gt, b1), vint8_and(eq, vint8_not8(b1)));
           
            final = vint8_add(updt, b1);
            final = vint8_and(final, vint8_lt(final, two));

            /* Storing results */
            vint8_store2D(board, i, j, final);

            /* Register rotation */
            a0 = b0;
            a1 = b1;
            a2 = b2;

            b0 = c0;
            b1 = c1;
            b2 = c2; 
        }
    }

    free_board(board_c, height, width);
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

