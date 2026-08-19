#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>

#define DELAY 17
#define WIDTH 800
#define HIGHT 600
#define SMALL 50
#define LARGE 70

void move(SDL_Event *e, int *x, int *y);

int main(int argc, char* argv) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Ошибка инициализации SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Квадрат в квадрате", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WIDTH, 
        HIGHT, 
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "Ошибка создания окна: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Ошибка создания рендерера: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    int running = 1;
    SDL_Event event;
    int x = 100, y = 100;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
            else if (event.type == SDL_KEYDOWN)
                move(&event, &x, &y);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect big = {333, 444, LARGE, LARGE};
        SDL_RenderFillRect(renderer, &big);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect small = {x, y, SMALL, SMALL};
        SDL_RenderFillRect(renderer, &small);

        SDL_RenderPresent(renderer);
        SDL_Delay(DELAY);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void move(SDL_Event *e, int *x, int *y)
{
    switch(e->key.keysym.sym) {
        case SDLK_RIGHT:
            *x += 10;
            break;
        case SDLK_LEFT:
            *x -= 10;
            break;
        case SDLK_UP:
            *y -= 10;
            break;
        case SDLK_DOWN:
            *y += 10;
            break;
        default:
            break;    
    }
}
