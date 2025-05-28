#include "Gem.hpp"
#include "GameGrid.hpp"

Gem::Gem(GemColor color, int size)
    : color(color) {
    rect = { 0, 0, size, size };
}

void Gem::render(SDL_Renderer* renderer, int x, int y) {
    setPosition(x, y);

    switch (color) {
    case GemColor::Red:    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 201); break;
    case GemColor::Blue:   SDL_SetRenderDrawColor(renderer, 0, 0, 255, 201); break;
    case GemColor::Green:  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 201); break;
    case GemColor::Yellow: SDL_SetRenderDrawColor(renderer, 255, 255, 0, 201); break;
    case GemColor::Purple: SDL_SetRenderDrawColor(renderer, 128, 0, 128, 201); break;
    case GemColor::Empty:  SDL_SetRenderDrawColor(renderer, 50, 50, 50, 201); break;
    case GemColor::BombColor: SDL_SetRenderDrawColor(renderer, 128, 32, 32, 201); break;
    case GemColor::ColorSpace: SDL_SetRenderDrawColor(renderer, 240, 240, 255, 201); break;
    }

    SDL_RenderFillRect(renderer, &rect);
}

bool Gem::contains(int mouseX, int mouseY) const {
    return (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
        mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

GemColor Gem::getColor() const {
    return color;
}

void Gem::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

void Gem::setColor(GemColor newColor) {
    color = newColor;
}

ColorizeGem::ColorizeGem(GemColor color, int size)
    : Gem(color, size) {}

void ColorizeGem::findTargetGems(const std::vector<std::vector<std::unique_ptr<Gem>>>& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {
    std::set<std::pair<int, int>> forbiddenCells(matches.begin(), matches.end());
    int width = grid.size();
    int height = grid[0].size();
    int radius = 3;

    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < width && newY >= 0 && newY < height && 
                (abs(dx) > 1 || abs(dy) > 1) && 
                grid[newX][newY]->getColor() != GemColor::Empty && 
                forbiddenCells.find({newX, newY}) == forbiddenCells.end()) {
                targetGems.emplace_back(newX, newY);
            }
        }
    }

    if (targetGems.size() > 3) {
        for (size_t i = 0; i < 3; ++i) {
            size_t j = i + rand() % (targetGems.size() - i);
            std::swap(targetGems[i], targetGems[j]);
        }
        targetGems.resize(3);
    }
}

void ColorizeGem::renderBonusEffect(SDL_Renderer* renderer, GameGrid& grid) {
    for (auto& target : targetGems) {
        grid.getGem(target.first, target.second).setColor(GemColor::ColorSpace);
    }
    grid.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);

    for (auto& target : targetGems) {
        grid.getGem(target.first, target.second).setColor(color);
    }
    grid.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(50);
}

void ColorizeGem::activateBonus(SDL_Renderer* renderer, GameGrid& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {
    findTargetGems(grid.getGrid(), x, y, matches);
    renderBonusEffect(renderer, grid);
}

BombGem::BombGem(GemColor color, int size)
    : Gem(color, size) {}

void BombGem::findTargetGems(const std::vector<std::vector<std::unique_ptr<Gem>>>& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {
    std::set<std::pair<int, int>> forbiddenCells(matches.begin(), matches.end());
    forbiddenCells.insert({x, y});
    int width = grid.size();
    int height = grid[0].size();
    int radius = 2;

    std::vector<std::pair<int, int>> potentialTargets;
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int newX = x + dx;
            int newY = y + dy;

            if (newX >= 0 && newX < width && newY >= 0 && newY < height && 
                grid[newX][newY]->getColor() == color &&
                grid[newX][newY]->getColor() != GemColor::Empty && 
                dynamic_cast<BombGem*>(grid[newX][newY].get()) == nullptr &&
                forbiddenCells.find({newX, newY}) == forbiddenCells.end()) {
                potentialTargets.emplace_back(newX, newY);
            }
        }
    }

    if (potentialTargets.size() > 5) {
        for (size_t i = 0; i < 5; ++i) {
            size_t j = i + rand() % (potentialTargets.size() - i);
            std::swap(potentialTargets[i], potentialTargets[j]);
        }
        potentialTargets.resize(5);
    }

    targetGems = std::move(potentialTargets);
}

void BombGem::renderBonusEffect(SDL_Renderer* renderer, GameGrid& grid) {
    for (auto& target : targetGems) {
        grid.getGem(target.first, target.second).setColor(GemColor::BombColor);
    }
    grid.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);

    for (auto& target : targetGems) {
        grid.getGem(target.first, target.second).setColor(GemColor::Empty);
    }
    grid.render(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
}

void BombGem::activateBonus(SDL_Renderer* renderer, GameGrid& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {
    findTargetGems(grid.getGrid(), x, y, matches);
    renderBonusEffect(renderer, grid);
}

