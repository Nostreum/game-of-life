#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define DEAD      0
#define ALIVE     1

#define SHAPE_MAX_SIZE 200

#define FPS_INTERVAL 1.0

#define CELL_SIZE 4

typedef struct param_list_s {

  int height;
  int width;
  uint64_t nb_max_gen;
  int period;
  bool pause;
  bool debug;
  char *filename;

} param_list_t;

void init_param_list(param_list_t *p){

  p->height = 16;
  p->width  = 16;
  p->nb_max_gen = 1000000L;
  p->period = 1000;
  p->pause = false;
  p->debug = false;
  p->filename = "shape.txt";
}

int** alloc_board(int height, int width){

  int **board = (int**) malloc(height * sizeof(int*));

  for(int i=0; i<height; i++){
    board[i] = (int*) malloc(width * sizeof(int));

    for(int j=0; j<width; j++)
      board[i][j] = DEAD;
  }

  return board;
}

int **copy_board(int **board, int height, int width){

  int **board_c = alloc_board(height, width);

  for(int i=0; i<height; i++)
    for(int j=0; j<width; j++)
      board_c[i][j] = board[i][j];

  return board_c;
}

void read_file(char *filename, int *h, int *w, int shape[SHAPE_MAX_SIZE][SHAPE_MAX_SIZE]){

  FILE *f = fopen(filename, "r");

  if(!f){
    printf("Can not open file %s \n", filename);
    exit(0);
  }

  printf("Reading config file : %s \n", filename);

  int nb_read = 0;

  for(int i=0; i<SHAPE_MAX_SIZE; i++){
    for(int j=0; j<SHAPE_MAX_SIZE; j++){
      fscanf(f, "%1d", &nb_read); 
      
      if(nb_read == 2){
        *w = j > *w ? j : *w;
        break;
      }
      else if(nb_read == 3){
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

//TODO: Pass different patterns in parameter (using a text file for example ?)
void init_board(int **board, int height, int width, char *filename){
  
  int h, w;
  int shape[SHAPE_MAX_SIZE][SHAPE_MAX_SIZE];

  int oi = width/4;
  int oj = height/2;

  read_file(filename, &h, &w, shape);  
 
  for(int i = 0; i<h; i++){
    for(int j=0; j<w; j++){
      board[height/2 + j][width/2 + i] = shape[i][j];
    }
  }
}

void next_generation(int **board, int height, int width){

  int **board_c = copy_board(board, height, width);

  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){

      if(i==0 || j==0 || i==height-1 || j==width-1) continue;

      int cs = board_c[i][j];

      int sum_dead  = 0;
      int sum_alive = 0;
      int count=0;
      for(int k=i-1; k<=i+1; k++){
        for(int l=j-1; l<=j+1; l++){
          if(k == i && l == j) continue;
          
          if      (board_c[k][l] == DEAD)   sum_dead++;
          else if (board_c[k][l] == ALIVE)  sum_alive++;
        }
      }

      // Cell is alive
      if(cs == ALIVE){
        // Under or over population
        if(sum_alive < 2 || sum_alive > 3)
          board[i][j] = DEAD;
      }
      else if(cs == DEAD){
        if(sum_alive == 3)
          board[i][j] = ALIVE;
      }
      else{
        printf("Cell (%d, %d) has a wrong state (%d). Quiting. \n", i, j, cs);
        exit(0); 
      }
    }
  }

  for(int i=0; i<height; i++)
    free(board_c[i]);

  free(board_c);
} 

void show_board(int **board, int height, int width){

  for(int i=0; i<height; i++){
    printf("|");
    for(int j=0; j<width; j++){
      printf(" %d |", board[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

SDL_Rect create_rect(int x, int y){

  SDL_Rect r;
  r.x = x * CELL_SIZE;
  r.y = y * CELL_SIZE;
  r.w = CELL_SIZE;
  r.h = CELL_SIZE;
  return r;
}

void print_help(){

  printf("Usage: life [option] \n");
  printf("Options: \n");
  printf("  -h          [Nb of row] \n");
  printf("  -w          [Nb of column] \n");
  printf("  -max_gen    [Maximum number of generation] \n");
  printf("  -period     [Time between 2 generations (in ms)] \n");
  printf("  -file       [Shape file (must be in shape folder)] \n");
  printf("  -debug      : Debug mode \n");
  printf("  -h          : Display this information \n");
  printf("\n\n");
  printf("Shape file synthax : \n");
  printf("  0 : Dead cell \n");
  printf("  1 : Alive cell \n");
  printf("  2 : End of row \n");
  printf("  3 : End of column (just need 1) \n");
  printf("\n");
  printf("Example with a square: \n");
  printf("  1 1 2 \n");
  printf("  1 1 2 \n");
  printf("  3     \n");
  printf("\n\n");

  exit(0);
}

void print_param_list(param_list_t p){

  printf("Params: \n");
  printf("  Height     : %d \n", p.height);
  printf("  Width      : %d \n", p.width);
  printf("  Nb max gen : %lu \n", p.nb_max_gen);
  printf("  Period     : %d \n", p.period);
  printf("  Debug      : %s \n", p.debug ? "True" : "False");
  printf("\n");

}

int main(int argc, char *argv[]){

  if(SDL_Init(SDL_INIT_VIDEO) == -1){
    printf("Could not initialise SDL \n");
    exit(0);  
  }

  param_list_t p;
  init_param_list(&p);

  for(int i=1; i<argc; i+=2){
    if(strcmp(argv[i], "-h") == 0 && argc > i + 1)
      p.height = atoi(argv[i+1]);
    else if(strcmp(argv[i], "-w") == 0 && argc > i + 1)
      p.width = atoi(argv[i+1]);
    else if(strcmp(argv[i], "-max_gen") == 0 && argc > i + 1)
      p.nb_max_gen = atoi(argv[i+1]);
    else if(strcmp(argv[i], "-period") == 0 && argc > i + 1)
      p.period = atoi(argv[i+1]);
    else if(strcmp(argv[i], "-debug") == 0)
      p.debug = true;
    else if(strcmp(argv[i], "-file") == 0 && argc > i + 1)
      p.filename = argv[i+1];
    else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
      print_help();
  } 

  print_param_list(p);

  int **board = alloc_board(p.height, p.width);
  struct timespec t1, t2;
  uint64_t diff;
  clock_gettime(CLOCK_MONOTONIC, &t1);  

  char *filename_full = malloc(strlen("shape/") + strlen(p.filename));
  strcpy(filename_full, "shape/");
  strcat(filename_full, p.filename);
  init_board(board, p.height, p.width, filename_full);

  uint32_t fps_t1, fps_t2;
  uint32_t fps_frames = 0;

  fps_t1 = SDL_GetTicks();

  SDL_Window *window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Rect rect[p.height][p.width];
 

  window = SDL_CreateWindow("Life", 0, 0, p.height * CELL_SIZE, p.width * CELL_SIZE, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, 0, 0);

  printf("Starting evolution... \n");

  for(int i=0; i<p.nb_max_gen; i++){


    clock_gettime(CLOCK_MONOTONIC, &t2); 
    diff = ( t2.tv_nsec - t1.tv_nsec ) / 1000000L + ( t2.tv_sec - t1.tv_sec ) * 1000L;  

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if(p.debug){
      printf("GENERATION : %d \n", i);
      show_board(board, p.height, p.width);
    }

    for(int i=0; i<p.height; i++){
      for(int j=0; j<p.width; j++){
        rect[i][j] = create_rect(i, j);
        if(board[i][j] == DEAD)
          SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        else
          SDL_SetRenderDrawColor(renderer, 0,0,0,255);
      
        SDL_RenderFillRect(renderer, &rect[i][j]);
      }
    } 
  
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT){  
        exit(0);
      }

      switch(event.type){
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym){
            case SDLK_SPACE:
              p.pause = p.pause ? false : true;
              break;
          } 
          break;
      }
    }

    fps_frames++;
    if(fps_t1 < SDL_GetTicks() - FPS_INTERVAL * 1000){
      fps_t1 = SDL_GetTicks();
      fps_t2 = fps_frames;
      printf("FPS = %u \n", fps_frames);
      fps_frames = 0;
    }

    SDL_RenderPresent(renderer); 
    if(!p.pause && diff > p.period){
      t1 = t2;
      next_generation(board, p.height, p.width);
    }
    else{
      i--;
    }
  }

  SDL_Quit();

  return 0;
}
