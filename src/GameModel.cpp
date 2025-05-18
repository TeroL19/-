#include "GameModel.hpp"

GameModel::GameModel()
    : grid(WINDOW_WIDTH, WINDOW_HEIGHT, BLOCK_SIZE, GRID_BUFFER)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL init error: %s", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow("Gem Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return;
    }
}

GameModel::~GameModel() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    SDL_Quit();
}
