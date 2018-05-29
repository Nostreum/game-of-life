#include <board_sdl.h>

board_SDL_t *SDL_init_board(int height, int width){

    board_SDL_t *board_sdl = (board_SDL_t*) malloc(sizeof(board_SDL_t));

    board_sdl->window       = NULL;
    board_sdl->renderer     = NULL;

    board_sdl->rect         = (SDL_Rect**) malloc(height * sizeof(SDL_Rect*));
    for (int i = 0; i<height; i++)
        board_sdl->rect[i]  = (SDL_Rect*) malloc(width * sizeof(SDL_Rect));

    return board_sdl;
}

void render_board_sdl(board_SDL_t *board_sdl, int **board, int x, int y, int h, int w, int cs){

    for (int i=x; i<h; i++) {
        for (int j=y; j<w; j++) {
            board_sdl->rect[i-x][j-y] = create_rect(i-x, j-y, cs);
            if (board[i][j] == DEAD){
                SDL_SetRenderDrawColor(board_sdl->renderer, 0,0,0,255);
                SDL_RenderDrawRect(board_sdl->renderer, &(board_sdl->rect[i-x][j-y]));
            }
            else {
                SDL_SetRenderDrawColor(board_sdl->renderer, 0,0,0,255);
                SDL_RenderFillRect(board_sdl->renderer, &(board_sdl->rect[i-x][j-y]));
            }
        }
    }   
}

SDL_Rect create_rect(int x, int y, int cs) {

  SDL_Rect r;
  r.x = y * cs;
  r.y = x * cs;
  r.w = cs;
  r.h = cs;
  return r;
}

