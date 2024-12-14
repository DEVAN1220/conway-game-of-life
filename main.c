#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
const int WINDOW_SIZE = 400;

int frame = 0;

bool *cells, *nextcells, isRunning;
SDL_FRect *rects;
int width, height, size;

void start(int cell_size) {
  width = WINDOW_SIZE / cell_size;
  height = WINDOW_SIZE / cell_size;
  size = cell_size;

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

bool getCell(int cellx, int celly) {
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

void update() {
  if (!isRunning)
    return;
  memset(nextcells, false, sizeof(bool) * height * width);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int neighbours = 0;
      int current = y * height + x;

      neighbours += getCell(x - 1, y - 1) == true ? 1 : 0;
      neighbours += getCell(x, y - 1) == true ? 1 : 0;
      neighbours += getCell(x + 1, y - 1) == true ? 1 : 0;
      neighbours += getCell(x - 1, y) == true ? 1 : 0;
      neighbours += getCell(x + 1, y) == true ? 1 : 0;
      neighbours += getCell(x - 1, y + 1) == true ? 1 : 0;
      neighbours += getCell(x, y + 1) == true ? 1 : 0;
      neighbours += getCell(x + 1, y + 1) == true ? 1 : 0;

      if (cells[current] == true) {
        if (neighbours < 2) {
          nextcells[current] = false;
        } else if (neighbours > 3) {
          nextcells[current] = false;
        } else {
          nextcells[current] = true;
        }
      } else if (neighbours == 3) {

        nextcells[current] = true;
      }
    }
  }
  memcpy(cells, nextcells, sizeof(bool) * height * width);
}

void clear() { memset(nextcells, false, sizeof(bool) * height * width); }

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  start(10);
  SDL_SetAppMetadata("conways game of life", "1.0", "com.example.CATEGORY-NAME");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("conways game of life", WINDOW_SIZE, WINDOW_SIZE, 0, &window, &renderer)) {
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
      isRunning = !isRunning;
      printf("%i", isRunning);
    }

    if (event->key.scancode == SDL_SCANCODE_C) {
      clear();
    }
  }

  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
      event->button.button == SDL_BUTTON_LEFT) {
    setCell(event->motion.x, event->motion.y);
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  frame++;

  if (frame > 10) {
    // printf("updated\n");
    update();
    frame = 0;
  }

  SDL_SetRenderDrawColor(renderer, 16, 16, 16, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  draw();
  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}