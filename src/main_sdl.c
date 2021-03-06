#include <global.h>
#include <board.h>
#include <board_sdl.h>
#include <text.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#define MOVE_SPEED              2
#define X_OFFSCREEN(max)        if(x < 0)   x = 0; \
                                if(x > max) x = max
#define Y_OFFSCREEN(max)        if(y < 0)   y = 0; \
                                if(y > max) y = max
#define WINDOW_SIZE_W           1280
#define WINDOW_SIZE_H           720

int main_sdl(int **board, param_list_t p){
   
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Could not initialise SDL \n");
        exit(0);  
    }

    if (TTF_Init() == -1) {
        printf("Could not initialise TTF \n");
        exit(0);  
    }

    struct timespec t1, t2;
    uint64_t diff;
    clock_gettime(CLOCK_MONOTONIC, &t1); 
    
    uint32_t fps_t1;
    uint32_t fps_frames = 0;
    fps_t1 = SDL_GetTicks();

    board_SDL_t *board_sdl  = SDL_init_board(p.height, p.width);
    board_sdl->window       = SDL_CreateWindow("Life", 0, 0, WINDOW_SIZE_W, WINDOW_SIZE_H, SDL_WINDOW_SHOWN);
    board_sdl->renderer     = SDL_CreateRenderer(board_sdl->window, 0, 0);

    printf("Starting evolution... \n");

    int nb_case_x = WINDOW_SIZE_W / p.cell_size;
    int nb_case_y = WINDOW_SIZE_H / p.cell_size;
    int x = p.width  / 2  - nb_case_x;
    int y = p.height / 2  - nb_case_y;
    int nx = nb_case_x;
    int ny = nb_case_y;

    int clic_x = 0;
    int clic_y = 0;

    sdl_text_t *text_gen = init_sdl_text(board_sdl->renderer, 0, 0, "Generation = 0", "font/OSB.ttf");
    sdl_text_t *text_fps = init_sdl_text(board_sdl->renderer, 0, 35, "FPS = X", "font/OSB.ttf");
   
    char gt[50];

    int gen=0;
    while (gen<p.nb_max_gen) {

        clock_gettime(CLOCK_MONOTONIC, &t2); 
        diff = ( t2.tv_nsec - t1.tv_nsec ) / 1000000L + ( t2.tv_sec - t1.tv_sec ) * 1000L;    

        SDL_SetRenderDrawColor(board_sdl->renderer, 255, 255, 255, 255);
        SDL_RenderClear(board_sdl->renderer);

        if (p.debug) {
            printf("GENERATION : %d \n", gen);
            show_board(board, p.height, p.width);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {    
                exit(0);
            }

            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_SPACE:
                            p.pause = p.pause ? false : true;
                            break;
                        case SDLK_RIGHT:
                            x += MOVE_SPEED;
                            break;
                        case SDLK_LEFT:
                            x -= MOVE_SPEED;
                            break;
                        case SDLK_UP:
                            y -= MOVE_SPEED;
                            break;
                        case SDLK_DOWN:
                            y += MOVE_SPEED;
                            break;
                        case SDLK_a:
                            p.period -= 50;
                            if (p.period <= 10) p.period = 10;
                            break;
                        case SDLK_s:
                            p.period += 50;
                            break;
                        case SDLK_c:
                            clean_board(board, p.height, p.width);
                            break;
                        case SDLK_KP_MINUS:
                            p.cell_size -= 1;
                            nx = WINDOW_SIZE_W / p.cell_size;
                            ny = WINDOW_SIZE_H / p.cell_size;
                            break;
                        case SDLK_KP_PLUS:
                            p.cell_size += 1;
                            nx = WINDOW_SIZE_W / p.cell_size;
                            ny = WINDOW_SIZE_H / p.cell_size;
                            break;
                        default:
                            break;
                    } 
                    break;
                case SDL_MOUSEMOTION:
                    switch (event.button.button) {
                        case SDL_BUTTON_LMASK:
                            clic_x = (event.motion.x / p.cell_size) + x;
                            clic_y = (event.motion.y / p.cell_size) + y;
                            update_board(board, clic_x, clic_y, ALIVE); 
                            break;
                        case SDL_BUTTON_RMASK: 
                            clic_x = (event.motion.x / p.cell_size) + x;
                            clic_y = (event.motion.y / p.cell_size) + y;
                            update_board(board, clic_x, clic_y, DEAD); 
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        X_OFFSCREEN(p.height - MOVE_SPEED);
        Y_OFFSCREEN(p.width  - MOVE_SPEED); 
        int nb_cell_x = ( x + nx > p.width  ) ? p.width  : x + nx;
        int nb_cell_y = ( y + ny > p.height ) ? p.height : y + ny;
        render_board_sdl(board_sdl, board, x, y, nb_cell_x, nb_cell_y, p.cell_size);

        fps_frames++;
        if (fps_t1 < SDL_GetTicks() - FPS_INTERVAL * 1000) {
            fps_t1 = SDL_GetTicks();
            printf("FPS = %d \n", fps_frames);
            
            sprintf(gt, "FPS = %d", fps_frames);
            update_sdl_text(board_sdl->renderer, text_fps, gt); 
        
            fps_frames = 0;
        }

        sprintf(gt, "Generation = %d", gen);
        update_sdl_text(board_sdl->renderer, text_gen, gt);

        SDL_RenderCopy(board_sdl->renderer, text_fps->texture, NULL, &text_fps->rect);
        SDL_RenderCopy(board_sdl->renderer, text_gen->texture, NULL, &text_gen->rect);
        SDL_RenderPresent(board_sdl->renderer); 
        if (!p.pause && diff > p.period) {
            t1 = t2;
            next_generation(board, p.height, p.width);
            gen++;
        }
    }

    SDL_Quit();
   
    return 0;
}
