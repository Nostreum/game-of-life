#ifndef TEXT_H_
#define TEXT_H_

#include <global.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct sdl_text_s {

    SDL_Surface *surface;
    SDL_Color   color;
    SDL_Color   bgcolor;
    SDL_Texture *texture;
    SDL_Rect    rect;
    TTF_Font    *font;
    
} sdl_text_t;

sdl_text_t* init_sdl_text(SDL_Renderer *renderer, int x, int y, char *text, char *fontname);
void update_sdl_text(SDL_Renderer *renderer, sdl_text_t *t, char *text);

#endif
