#include "Gem.hpp"

Gem::Gem(GemColor color, BonusType bonus, int size)
    : color(color), bonus(bonus) {
    rect = { 0, 0, size, size };

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(1, 100);

    if (bonus == BonusType::None) {
        int chance = dist(gen);
        if (chance <= 3) { 
            this->bonus = BonusType::Colorize;
        }
        else if (chance <= 5) {
            this->bonus = BonusType::Bomb;
        }
    }
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

BonusType Gem::getBonusType() const {
    return bonus;
}

void Gem::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

void Gem::setColor(GemColor newColor) {
    color = newColor;
}

void Gem::setBonus(BonusType newBonus) {
    bonus = newBonus;
}
