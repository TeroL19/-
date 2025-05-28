#include "GameGrid.hpp"

GameGrid::GameGrid(int windowWidth, int windowHeight, int gemSize, int buffer) 
    : windowWidth(windowWidth), windowHeight(windowHeight), gemSize(gemSize), buffer(buffer) {
    gen.seed(std::random_device()());
    calculateSizes();
    grid.resize(gridWidth);
    for (auto& column : grid) {
        column.resize(gridHeight);
        for (auto& cell : column) {
            cell = std::make_unique<Gem>(GemColor::Empty, gemSize);
        }
    }
    init();
}

void GameGrid::init() {
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            std::vector<GemColor> exclude;

            if (x >= 2 && grid[x - 1][y]->getColor() == grid[x - 2][y]->getColor()) {
                exclude.push_back(grid[x - 1][y]->getColor());
            }

            if (y >= 2 && grid[x][y - 1]->getColor() == grid[x][y - 2]->getColor()) {
                exclude.push_back(grid[x][y - 1]->getColor());
            }

            randomGem(x, y, exclude);
        }
    }
}

void GameGrid::render(SDL_Renderer* renderer) {
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            int drawX = x * (gemSize + buffer) + buffer;
            int drawY = y * (gemSize + buffer) + buffer;
            grid[x][y]->render(renderer, drawX, drawY);
        }
    }
}

std::pair<int, int> GameGrid::getSize() const {
    return std::pair<int, int>(gridWidth, gridHeight);
}

Gem& GameGrid::getGem(int x, int y) {
    return *grid[x][y];
}

bool GameGrid::swapGems(int x1, int y1, int x2, int y2) {
    if ((abs(x1 - x2) == 1 && y1 == y2) || (abs(y1 - y2) == 1 && x1 == x2)) {
        grid[x1][y1].swap(grid[x2][y2]);
        return true;
    }
    return false;
}

bool GameGrid::checkMatches() {
    auto matches = findMatches();
    return !matches.empty();
}

bool GameGrid::applyGravity() {
    bool changed = false;

    for (int x = 0; x < gridWidth; ++x) {
        int changeY = gridHeight - 1; 

        for (int y = gridHeight - 1; y >= 0; --y) {
            if (grid[x][y]->getColor() != GemColor::Empty) {
                if (y != changeY) {
                    grid[x][changeY] = std::move(grid[x][y]);
                    grid[x][y] = std::make_unique<Gem>(GemColor::Empty, gemSize);
                    changed = true;
                }
                --changeY;
            }
        }

        for (int y = changeY; y >= 0; --y) {
            randomGem(x, y);
            changed = true;
        }
    }

    return changed;
}

void GameGrid::randomGem(int x, int y, const std::vector<GemColor>& exclude) {
    std::unordered_set<GemColor> excludeSet(exclude.begin(), exclude.end());

    std::vector<GemColor> allowed;
    allowed.reserve(static_cast<int>(GemColor::Purple) + 1);

    for (int i = 0; i <= static_cast<int>(GemColor::Purple); ++i) {
        GemColor color = static_cast<GemColor>(i);
        if (excludeSet.find(color) == excludeSet.end()) {
            allowed.push_back(color);
        }
    }

    if (allowed.empty()) {
        grid[x][y] = std::make_unique<Gem>(GemColor::Empty, gemSize);
    }
    else {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(allowed.size()) - 1);
        GemColor selectedColor = allowed[dist(gen)];
        
        std::uniform_int_distribution<int> bonusDist(1, 100);
        int chance = bonusDist(gen);
        
        if (chance <= 3) {
            grid[x][y] = std::make_unique<ColorizeGem>(selectedColor, gemSize);
        }
        else if (chance <= 5) {
            grid[x][y] = std::make_unique<BombGem>(selectedColor, gemSize);
        }
        else {
            grid[x][y] = std::make_unique<Gem>(selectedColor, gemSize);
        }
    }
}

void GameGrid::calculateSizes() {
    gridWidth = (windowWidth - buffer) / (gemSize + buffer);
    gridHeight = (windowHeight - buffer) / (gemSize + buffer);

    if (gridWidth < 3) gridWidth = 3;
    if (gridHeight < 3) gridHeight = 3;
}

std::vector<std::pair<int, int>> GameGrid::findMatches() const {
    std::vector<std::pair<int, int>> answer;

    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            GemColor cur = grid[x][y]->getColor();
            GemColor cur_dx = GemColor::Empty;
            GemColor cur_2_dx = GemColor::Empty;
            GemColor cur_dy = GemColor::Empty;
            GemColor cur_2_dy = GemColor::Empty;
            if (x >= 2) {
                cur_dx = grid[x - 1][y]->getColor();
                cur_2_dx = grid[x - 2][y]->getColor();
            }
            if (y >= 2) {
                cur_dy = grid[x][y - 1]->getColor();
                cur_2_dy = grid[x][y - 2]->getColor();
            }

            if ((cur == cur_dx) && (cur == cur_2_dx)) {
                answer.emplace_back(x, y);
                answer.emplace_back(x - 1, y);
                answer.emplace_back(x - 2, y);
            }

            if ((cur == cur_dy) && (cur == cur_2_dy)) {
                answer.emplace_back(x, y);
                answer.emplace_back(x, y - 1);
                answer.emplace_back(x, y - 2);
            }
        }
    }
    return answer;
}

void GameGrid::removeMatches(SDL_Renderer* renderer, const std::vector<std::pair<int, int>>& matches) {
    for (auto& match : matches) {
        int x = match.first;
        int y = match.second;
        if (dynamic_cast<ColorizeGem*>(grid[x][y].get()) != nullptr || 
            dynamic_cast<BombGem*>(grid[x][y].get()) != nullptr) {
            grid[x][y]->activateBonus(renderer, *this, x, y, matches);
        }
    }
    
    for (auto& match : matches) {
        int x = match.first;
        int y = match.second;
        grid[x][y].reset(new Gem(GemColor::Empty, gemSize));
    }
}

SDL_Point GameGrid::gridToPixel(int gridX, int gridY) {
    SDL_Point pixelPos;
    pixelPos.x = gridX * (gemSize + buffer) + buffer;
    pixelPos.y = gridY * (gemSize + buffer) + buffer;
    return pixelPos;
}

std::pair<int, int> GameGrid::pixelToGrid(int pixelX, int pixelY) {
    int gridX = (pixelX - buffer) / (gemSize + buffer);
    int gridY = (pixelY - buffer) / (gemSize + buffer);

    if (gridX < 0) gridX = 0;
    if (gridY < 0) gridY = 0;
    if (gridX >= gridWidth) gridX = gridWidth - 1;
    if (gridY >= gridHeight) gridY = gridHeight - 1;

    return { gridX, gridY };
}