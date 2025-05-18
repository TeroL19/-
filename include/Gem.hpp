#pragma once

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

enum class BonusType { None, Colorize, Bomb };

class Gem {
public:
    Gem(GemColor color = GemColor::Empty, BonusType bonus = BonusType::None, int size = 32);

    void render(SDL_Renderer* renderer, int x, int y);

    bool contains(int mouseX, int mouseY) const;

    GemColor getColor() const;
    BonusType getBonusType() const;

    void setPosition(int x, int y);
    void setColor(GemColor color);
    void setBonus(BonusType bonus);

private:
    GemColor color;
    BonusType bonus;
    SDL_Rect rect;
};