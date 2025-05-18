#include "Bonus.hpp"
#include "GameGrid.hpp"

std::vector<std::pair<int, int>> Bonus::getValidCellsInRadius(const std::vector<std::vector<Gem>>& grid, int targetX, int targetY, int radius) {
    std::vector<std::pair<int, int>> result;
    int width = grid.size();
    int height = grid[0].size();

    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int x = targetX + dx;
            int y = targetY + dy;

            std::pair<int, int> currentPair;
            currentPair.first = x;
            currentPair.second = y;

            if (x >= 0 && x < width && y >= 0 && y < height && (abs(dx) > 1 || abs(dy)) && 
                grid[x][y].getColor() != GemColor::Empty && (!(forbiddenCells.find(currentPair) != forbiddenCells.end()))) {
                result.emplace_back(x, y);
            }
        }
    }
    return result;
}

void Bonus::findGems(std::vector<std::vector<Gem>>& grid, int targetX, int targetY) {
    std::vector<std::pair<int, int>> candidates = Bonus::getValidCellsInRadius(grid, targetX, targetY);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(candidates.begin(), candidates.end(), gen);

    for (int i = 0; i < count; i++) {
        std::pair<int, int> current = candidates[i];
        targets.emplace_back(current);
    }
}

void ColorizeBonus::render(SDL_Renderer* renderer, GameGrid& gridClass) {
    for (auto& target : targets) {
        int x = target.first;
        int y = target.second;

        gridClass.getGem(x, y).setColor(GemColor::ColorSpace);
    }
    gridClass.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);

    for (auto& target : targets) {
        int x = target.first;
        int y = target.second;

        gridClass.getGem(x, y).setColor(startColor);
        gridClass.render(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(50);
    }
}

void BombBonus::render(SDL_Renderer* renderer, GameGrid& gridClass) {
    for (auto& target : targets) {
        int x = target.first;
        int y = target.second;
        gridClass.getGem(x, y).setColor(GemColor::BombColor);
    }
    gridClass.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);

    for (auto& target : targets) {
        int x = target.first;
        int y = target.second;
        gridClass.getGem(x, y).setColor(GemColor::Empty);
    }

    gridClass.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}

void Bonus::renderCycle(SDL_Renderer* renderer, GameGrid& gridClass, int cycles) {
    for (int i = 0; i < cycles; ++i) {
        render(renderer, gridClass);
    }
}
