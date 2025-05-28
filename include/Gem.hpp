#pragma once

#include <iostream>
#include <random>
#include <set>
#include <unordered_set>
#include <memory>
#include <optional>
#include <vector>
#include <algorithm>
#include <SDL_log.h>

#include <SDL.h>

enum class GemColor { Red, Blue, Green, Yellow, Purple, Empty, BombColor, ColorSpace};

class GameGrid;

class Gem {
    friend class GameGrid;

public:
    Gem(GemColor color = GemColor::Empty, int size = 32);
    virtual ~Gem() = default;

    virtual void render(SDL_Renderer* renderer, int x, int y);
    virtual bool contains(int mouseX, int mouseY) const;
    virtual GemColor getColor() const;
    virtual void setPosition(int x, int y);
    virtual void setColor(GemColor color);

    virtual void activateBonus(SDL_Renderer* renderer, GameGrid& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {}
    virtual void findTargetGems(const std::vector<std::vector<std::unique_ptr<Gem>>>& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) {}
    virtual void renderBonusEffect(SDL_Renderer* renderer, GameGrid& grid) {}

protected:
    GemColor color;
    SDL_Rect rect;
    std::vector<std::pair<int, int>> targetGems;
};

class ColorizeGem : public Gem {
public:
    ColorizeGem(GemColor color = GemColor::Empty, int size = 32);
    void activateBonus(SDL_Renderer* renderer, GameGrid& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) override;
    void findTargetGems(const std::vector<std::vector<std::unique_ptr<Gem>>>& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) override;
    void renderBonusEffect(SDL_Renderer* renderer, GameGrid& grid) override;
};

class BombGem : public Gem {
public:
    BombGem(GemColor color = GemColor::Empty, int size = 32);
    void activateBonus(SDL_Renderer* renderer, GameGrid& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) override;
    void findTargetGems(const std::vector<std::vector<std::unique_ptr<Gem>>>& grid, int x, int y, const std::vector<std::pair<int, int>>& matches) override;
    void renderBonusEffect(SDL_Renderer* renderer, GameGrid& grid) override;
};