#include <text.h>

sdl_text_t* init_sdl_text(SDL_Renderer *renderer, int x, int y, char *text, char *fontname){

    sdl_text_t *t = (sdl_text_t *) malloc(sizeof(sdl_text_t));

    t->color.r = 255;
    t->color.g = 0;
    t->color.b = 255;
    t->font  = TTF_OpenFont(fontname, 50);

    t->surface = TTF_RenderText_Solid(t->font, text, t->color);
    t->texture = SDL_CreateTextureFromSurface(renderer, t->surface);
    t->rect.x = x;
    t->rect.y = y;
    t->rect.w = t->surface->w;
    t->rect.h = t->surface->h;

    return t;
}

void update_sdl_text(SDL_Renderer *renderer, sdl_text_t *t, char *text){

    t->surface = TTF_RenderText_Solid(t->font, text, t->color);
    t->texture = SDL_CreateTextureFromSurface(renderer, t->surface);

}
