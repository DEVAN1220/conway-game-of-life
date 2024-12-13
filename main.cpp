#include <iostream>
#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
int frame = 0;

class game {
public:
  bool *cells;
  bool *nextcells;
  bool isRunning;
  SDL_FRect *rects;
  const int width, height, size;

public:
  game(int cell_size)
      : size(cell_size), width(WINDOW_WIDTH / cell_size),
        height((WINDOW_HEIGHT / cell_size)) {
    cells = (bool *)malloc(sizeof(bool) * height * width);
    nextcells = (bool *)malloc(sizeof(bool) * height * width);

    rects = (SDL_FRect *)malloc(sizeof(SDL_FRect) * height * width);

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        cells[y * height + x] = false;
        nextcells[y * height + x] = false;
        rects[y * height + x].x = x * cell_size;

        rects[y * height + x].y = y * cell_size;
        rects[y * height + x].h = cell_size;
        rects[y * height + x].w = cell_size;
        isRunning = false;
      }
    }
  }

  void draw() {

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        if (cells[y * height + x] == true) {
          SDL_SetRenderDrawColor(renderer, 235, 219, 178, 0);
          SDL_RenderFillRect(renderer, &rects[y * height + x]);
        }
      }
    }
  }

  bool getCell(int cellx, int celly){
    if (cellx >= 0 || cellx < width || celly >= 0 || celly < height) {
      return cells[celly * height + cellx];
    } else {
      return false;
    }
  }
  
  void setCell(int x, int y) {
    int cellx = x / size;
    int celly = y / size;
    cells[celly * height + cellx] = !cells[celly * height + cellx];
  }

  void update(){
    if (!isRunning) return;
    std::fill_n(nextcells, height * width, false);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        int neighbours = 0;
        int current = y * height + x;

        neighbours += getCell(x-1, y-1) == true ? 1 : 0;
        neighbours += getCell(x, y-1) == true ? 1 : 0;
        neighbours += getCell(x+1, y-1) == true ? 1 : 0;
        neighbours += getCell(x-1, y) == true ? 1 : 0;
        neighbours += getCell(x + 1, y) == true ? 1 : 0;
        neighbours += getCell(x - 1, y + 1) == true ? 1 : 0;
        neighbours += getCell(x  , y + 1) == true ? 1 : 0;
        neighbours += getCell(x + 1, y + 1) == true ? 1 : 0;
        if (neighbours >  10) {
        
        std::cout << x << ", " << y << std::endl;
        }

        if (cells[current] == true) {
          if (neighbours < 2) {
            nextcells[current] = false;
            //std::cout << "died of underpopulation\n";
          } else if (neighbours > 3) {
            nextcells[current] = false;
            //std::cout << "died of overpopulation\n";
          } else {
            nextcells[current] = true;
            //std::cout << "lived w ne.\n";

          }
        } else if (neighbours == 3) {
          //std::cout << "reproduction\n";

          nextcells[current] = true;
        }
      }
    }

    std::copy(nextcells, nextcells + (height * width), (cells));    
  }

  void clear() { std::fill_n(cells, height * width, false); }
};

game g(10);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("conways game of life", "1.0", NULL);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("conway game of life", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE; 
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; 
  }
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
      return SDL_APP_SUCCESS;
    }
    if (event->key.scancode == SDL_SCANCODE_S) {
      g.isRunning = !g.isRunning;
      std::cout << g.isRunning << std::endl;
    }

    if (event->key.scancode == SDL_SCANCODE_C) {
      g.clear();
      std::cout << "cleared" << std::endl;
    }
  }

  if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT) {
    g.setCell(event->motion.x, event->motion.y);
  } 
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
      event->button.button == SDL_BUTTON_LEFT) {
    g.setCell(event->motion.x, event->motion.y);
  }
  


  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  frame++;

  if (frame > 10) {
    g.update();
    frame = 0;
  }

  SDL_SetRenderDrawColor(renderer, 16, 16, 16,
                         SDL_ALPHA_OPAQUE); 
  SDL_RenderClear(renderer);                
  g.draw();
  SDL_RenderPresent(renderer); 
  return SDL_APP_CONTINUE; 
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
