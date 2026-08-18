#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREENWIDTH = 640;
const int SCREENHEIGHT = 480;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
int board = {0}; 
// 0 — пусто, 1 — X, 2 — O [9](https://codepal.ai/code-generator/query/Xdqy2F4N/create-tic-tac-toe-in-sdl-in-c)

int init() {
    puts("0");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        exit(1);
    }
    puts("1");
    
    gWindow = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Window could not be created! SDL_Error: %s", SDL_GetError());
        exit(1);
    }
    puts("2");
    
    // gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
    // if (gRenderer == NULL) {
    //     printf("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    //     exit(1);
    // }
    // puts("3");
    // SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    puts("4");
}

void drawBoard() {
    // Рисуем горизонтальные линии
    SDL_RenderDrawLine(gRenderer, 0, SCREENHEIGHT / 3, SCREENWIDTH, SCREENHEIGHT / 3);
    SDL_RenderDrawLine(gRenderer, 0, SCREENHEIGHT * 2 / 3, SCREENWIDTH, SCREENHEIGHT * 2 / 3);
}

void drawX(int row, int col) {
    // Здесь нужно отрисовать символ X в ячейке (row, col)
}

void drawO(int row, int col) {
    // И здесь — символ O
}

void close() {
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_Quit();
}

int main(int argc, char *argv) {
    init();
    puts("inited");
    return 0;
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                // Обработка клика мыши
            }
        }
        SDL_RenderClear(gRenderer);
        drawBoard();
        drawX(0, 0); // Пример: рисуем X в первой ячейке (нужно определить позицию)
        SDL_RenderPresent(gRenderer);
    }
    close();
    return 0;
}
// ``` 

// ## Советы по реализации

// *   **Обработка ввода.** При клике мыши нужно определить, в какую ячейку попал курсор, и проверить, не занята ли она.
// *   **Проверка победы.** После каждого хода проверяйте все возможные комбинации (три строки, три столбца, две диагонали).
// *   **Рисование.** Для отображения символов X и O используйте текстуры (SDL_Texture) из изображений (PNG), загружаемых через SDL_image.
// *   **Структура проекта.** Разбейте код на логические блоки: инициализация SDL, обработка событий, отрисовка поля, проверка победы. Это упростит отладку.

//  [1](https://github.com/Luffy-D-Zoro/interactive-gui-tic-tac-toe-c-sdl)[7](https://gamedev.net/forums/topic/573067-my-first-csdl-game-tic-tac-toe/)

// ## Готовые проекты

// Если хотите сразу посмотреть рабочий пример, советую изучить эти репозитории на GitHub:
// *   **[Luffy-D-Zoro/interactive-gui-tic-tac-toe-c-sdl](https://github.com/Luffy-D-Zoro/interactive-gui-tic-tac-toe-c-sdl)** — реализация на C с SDL2 и SDL_ttf. Есть интерактивное взаимодействие (наведение и клик мыши), система ячеек, распознавание победы и ничьей, кнопка «Играть снова».
// *   **[meet244/Tic-Tac-Toe](https://github.com/meet244/Tic-Tac-Toe)** — версия с возможностью игры против компьютера или другого игрока, управление через клики мыши.

//  [1](https://github.com/Luffy-D-Zoro/interactive-gui-tic-tac-toe-c-sdl)[6](https://github.com/meet244/Tic-Tac-Toe)

// ## Компиляция и запуск

// Для сборки потребуется установить SDL2 и его разработки пакеты (заголовочные файлы и библиотеки) для вашей ОС. При компиляции проекта не забудьте указать пути к библиотекам SDL и подключить SDL_image, SDL_ttf (если используете SDL_ttf). [6](https://github.com/meet244/Tic-Tac-Toe)[1](https://github.com/Luffy-D-Zoro/interactive-gui-tic-tac-toe-c-sdl)

// **Пример команды для GCC:**
// ```bash
// gcc -o tic_tac_toe tic_tac_toe.c -lSDL2 -lSDL2_image -lSDL2_ttf
