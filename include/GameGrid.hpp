#pragma once

#include <vector>
#include <random>
#include <memory>
#include <SDL.h>
#include "Gem.hpp"

class GameGrid {
public:
    GameGrid(int windowWidth, int windowHeight, int gemSize, int buffer);

    void init();

    void render(SDL_Renderer* renderer);

    std::pair<int, int> getSize() const;

    Gem& getGem(int x, int y);

    bool swapGems(int x1, int y1, int x2, int y2);

    bool checkMatches();

    std::vector<std::pair<int, int>> findMatches() const;

    void removeMatches(SDL_Renderer* renderer, const std::vector<std::pair<int, int>>& matches);

    bool applyGravity();

    void randomGem(int x, int y, const std::vector<GemColor>& exclude = {});

    SDL_Point gridToPixel(int gridX, int gridY);

    std::pair<int, int> pixelToGrid(int pixelX, int pixelY);

    const std::vector<std::vector<std::unique_ptr<Gem>>>& getGrid() const { return grid; }

private:
    void calculateSizes();

    int windowWidth;
    int windowHeight;
    int gemSize;
    int buffer;
    int gridWidth;
    int gridHeight;
    std::vector<std::vector<std::unique_ptr<Gem>>> grid;
    std::mt19937 gen;
};