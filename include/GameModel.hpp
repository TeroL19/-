#pragma once
#include "GameGrid.hpp"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

#define BLOCK_SIZE 40
#define GRID_BUFFER 4

struct AnimTimings {
    int click = 60;
    int match = 300;
    int fallStep = 200;
};

class GameModel {
public:
    GameModel();
    ~GameModel();

    SDL_Window* getWindow() const { return window; }
    SDL_Renderer* getRenderer() const { return renderer; }
    GameGrid& getGrid() { return grid; }
    const AnimTimings& getTimings() const { return timings; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    GameGrid grid;
    AnimTimings timings;
};
