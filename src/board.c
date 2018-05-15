#include <board.h>
#include <file.h>

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

void next_generation(int **board, int height, int width) {

    int **board_c = copy_board(board, height, width);

    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {

            if (i==0 || j==0 || i==height-1 || j==width-1) continue;

            int cs = board_c[i][j];

            int sum_dead  = 0;
            int sum_alive = 0;
            for (int k=i-1; k<=i+1; k++) {
                for (int l=j-1; l<=j+1; l++) {
                    if (k == i && l == j) continue;
          
                    if      (board_c[k][l] == DEAD)   sum_dead++;
                    else if (board_c[k][l] == ALIVE)  sum_alive++;
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

