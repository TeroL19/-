#pragma once
#include <vector>
#include "Gem.hpp"

class GameGrid;

class Bonus {
public:
    Bonus(Gem& current, const std::vector<std::pair<int, int>>& forbiddenCells, int count)
        : forbiddenCells(forbiddenCells.begin(), forbiddenCells.end()), startColor(current.getColor()), count(count) {}

    std::vector<std::pair<int, int>> getValidCellsInRadius(const std::vector<std::vector<Gem>>& grid, 
        int targetX, int targetY, int radius = 3);

    void findGems(std::vector<std::vector<Gem>>& grid, int targetX, int targetY);

    virtual void render(SDL_Renderer* renderer, GameGrid& gridClass) = 0;

    void renderCycle(SDL_Renderer* renderer, GameGrid& gridClass, int cycles = 3);

protected:
    std::vector<std::pair<int, int>> targets;
    std::set<std::pair<int, int>> forbiddenCells;
    GemColor startColor;
    int count;
};

class ColorizeBonus : public Bonus {
public:
    ColorizeBonus(Gem& current,
        const std::vector<std::pair<int, int>>& forbiddenCells)
        : Bonus(current, forbiddenCells, 3) {}

    void render(SDL_Renderer* renderer, GameGrid& gridClass) override;
};

class BombBonus : public Bonus {
public:
    BombBonus(Gem& current,
        const std::vector<std::pair<int, int>>& forbiddenCells)
        : Bonus(current, forbiddenCells, 5) {}

    void render(SDL_Renderer* renderer, GameGrid& gridClass) override;
};