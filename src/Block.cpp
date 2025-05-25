#include "Block.hpp"
#include <iostream>

Block::Block(int x, int y, int width, int height, SDL_Color color, int health)
    : rect{x, y, width, height}, color(color), health(health), destroyed(false) {}

void Block::render(SDL_Renderer* renderer) {
    if (!destroyed) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool Block::isDestroyed() const {
    return destroyed;
}

void Block::destroy() {
    destroyed = true;
}

int Block::findClosestPoint(int value, int min, int max) const {
    return std::max(min, std::min(value, max));
}

bool Block::isPointInRange(int point, int min, int max) const {
    return point >= min && point <= max;
}

int Block::calculateDistanceSquared(int x1, int y1, int x2, int y2) const {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

bool Block::checkCollision(int ballX, int ballY, int ballRadius) const {
    if (destroyed) return false;
    
    int closestX = findClosestPoint(ballX, rect.x, rect.x + rect.w);
    int closestY = findClosestPoint(ballY, rect.y, rect.y + rect.h);

    int distanceSquared = calculateDistanceSquared(ballX, ballY, closestX, closestY);
    
    return distanceSquared <= (ballRadius * ballRadius);
}

bool Block::isBlockDestroyed() const {
    return health <= 0;
}

int Block::calculatePoints(int damage) const {
    if (isBlockDestroyed()) {
        return 2; 
    }
    return 1; 
}

void Block::applyDamage(int damage) {
    health -= damage;
    if (isBlockDestroyed()) {
        destroy();
    }
}

int Block::handleCollision(int damage) {
    if (destroyed) return 0;
    
    applyDamage(damage);
    std::cout << "[DEBUG:Block.cpp] Block hit! Health: " << health << std::endl;
    return calculatePoints(damage);
}

UsualBlock::UsualBlock(int x, int y, int width, int height, SDL_Color color, int health)
    : Block(x, y, width, height, color, health) {}

int UsualBlock::handleCollision(int damage) {
    return Block::handleCollision(damage);
}

IndestructibleBlock::IndestructibleBlock(int x, int y, int width, int height)
    : Block(x, y, width, height, {128, 128, 128, 255}, 999999) {}

int IndestructibleBlock::handleCollision(int damage) {
    return 0;
}

SpeedBlock::SpeedBlock(int x, int y, int width, int height, SDL_Color color, int health)
    : Block(x, y, width, height, color, health) {}

int SpeedBlock::handleCollision(int damage) {
    std::cout << "[DEBUG:Block.cpp] Speed block hit! Health: " << health << std::endl;
    int points = Block::handleCollision(damage);
    return points * 2; 
}

BonusBlock::BonusBlock(int x, int y, int width, int height, SDL_Color color)
    : Block(x, y, width, height, color, 1) {}

int BonusBlock::handleCollision(int damage) {
    int points = Block::handleCollision(damage);
    return points * 3;
}