#ifndef BOARD_SDL_H_
#define BOARD_SDL_H_

#include <global.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

typedef struct board_SDL_s {

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect **rect;

} board_SDL_t;

board_SDL_t *SDL_init_board(int height, int width);
SDL_Rect create_rect(int x, int y, int cs);
void render_board_sdl(board_SDL_t *board_sdl, int **board, int x, int y, int height, int width, int cs);

#endif /* BOARD_SDL_H_ */
