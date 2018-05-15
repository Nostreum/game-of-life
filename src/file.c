#include <file.h>

void read_file(char *filename, int *h, int *w, int shape[PATTERN_MAX_SIZE][PATTERN_MAX_SIZE]) {

    FILE *f = fopen(filename, "r");

    if (!f) {
        printf("Can not open file %s \n", filename);
        exit(0);
    }

    printf("Reading config file : %s \n", filename);

    int nb_read = 0;

    for (int i=0; i<PATTERN_MAX_SIZE; i++) {
        for (int j=0; j<PATTERN_MAX_SIZE; j++) {
            int r = fscanf(f, "%1d", &nb_read); 
            
            if (nb_read == 2) {
                *w = j > *w ? j : *w;
                break;
            }
            else if (nb_read == 3) {
                *h = i > *h ? i : *h;
                goto end;
            }
            else{
                shape[i][j] = nb_read;
            }
        }    
    }
end:
    return;
}

