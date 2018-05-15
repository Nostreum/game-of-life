#include <global.h>
#include <board.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

SDL_Rect create_rect(int x, int y) {

  SDL_Rect r;
  r.x = x * CELL_SIZE;
  r.y = y * CELL_SIZE;
  r.w = CELL_SIZE;
  r.h = CELL_SIZE;
  return r;
}

int main_sdl(int **board, param_list_t p){
   
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Could not initialise SDL \n");
        exit(0);  
    }

    struct timespec t1, t2;
    uint64_t diff;
    clock_gettime(CLOCK_MONOTONIC, &t1); 
    
    uint32_t fps_t1, fps_t2;
    uint32_t fps_frames = 0;
    fps_t1 = SDL_GetTicks();

    SDL_Window *window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Rect rect[p.height][p.width];
 
    window = SDL_CreateWindow("Life", 0, 0, p.height * CELL_SIZE, p.width * CELL_SIZE, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, 0, 0);

    printf("Starting evolution... \n");

    for (int i=0; i<p.nb_max_gen; i++) {

        clock_gettime(CLOCK_MONOTONIC, &t2); 
        diff = ( t2.tv_nsec - t1.tv_nsec ) / 1000000L + ( t2.tv_sec - t1.tv_sec ) * 1000L;    

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (p.debug) {
            printf("GENERATION : %d \n", i);
            show_board(board, p.height, p.width);
        }

        for (int i=0; i<p.height; i++) {
            for (int j=0; j<p.width; j++) {
                rect[i][j] = create_rect(i, j);
                if (board[i][j] == DEAD)
                    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                else
                    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            
                SDL_RenderFillRect(renderer, &rect[i][j]);
            }
        } 
    
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {    
                exit(0);
            }

            switch(event.type) {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_SPACE:
                            p.pause = p.pause ? false : true;
                            break;
                    } 
                    break;
            }
        }

        fps_frames++;
        if (fps_t1 < SDL_GetTicks() - FPS_INTERVAL * 1000) {
            fps_t1 = SDL_GetTicks();
            fps_t2 = fps_frames;
            printf("FPS = %u \n", fps_frames);
            fps_frames = 0;
        }

        SDL_RenderPresent(renderer); 
        if (!p.pause && diff > p.period) {
            t1 = t2;
            next_generation(board, p.height, p.width);
        }
        else{
            i--;
        }
    }

    SDL_Quit();
   

}
