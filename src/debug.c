#include <debug.h>

void vint32_print(vint32 v, char *label) {

    int *vi = (int*)&v;
    printf("%s = [%d, %d, %d, %d] \n", label, vi[3], vi[2], vi[1], vi[0]);

}

void vint32_print_line(vint32 v0, vint32 v1, vint32 v2, char *label) {

    int *vi0 = (int*)&v0;
    int *vi1 = (int*)&v1;
    int *vi2 = (int*)&v2;

    printf("%s, [%d, %d, %d, %d] [%d, %d, %d, %d] [%d %d %d %d] \n", label, vi0[3], vi0[2], vi0[1], vi0[0], vi1[3], vi1[2], vi1[1], vi1[0], vi2[3], vi2[2], vi2[1], vi2[0]);    

}
