#include <global.h>
#include <board.h>
#include <main_sdl.h>


int main(int argc, char *argv[]) { 

    /* Params */
    param_list_t p;
    init_param_list(&p);
    read_param_cmd(argc, argv, &p);
    print_param_list(p);

    /* Board */
    int **board = alloc_board(p.height, p.width);
    init_board(board, p.height, p.width, p.filename);

    main_sdl(board, p); 

    /* TODO: Add OPENGL */
    
    return 0;
}
