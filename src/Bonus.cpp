#include "Bonus.hpp"
#include "Game.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

Bonus::Bonus(float x, float y, SDL_Color color) 
    : x(x), y(y), speedY(2.0f), active(true), color(color) {}

void Bonus::update() {
    updatePosition();
    if (isOutOfBounds()) {
        active = false;
    }
}

void Bonus::updatePosition() {
    y += speedY;
}

bool Bonus::isOutOfBounds() const {
    return y > Settings::HEIGHT;
}

void Bonus::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    renderBonusRect(renderer);
}

void Bonus::renderBonusRect(SDL_Renderer* renderer) const {
    SDL_Rect rect = {static_cast<int>(x - SIZE/2), static_cast<int>(y - SIZE/2), SIZE, SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

bool Bonus::checkCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight) {
    return isWithinPaddleBounds(paddleX, paddleY, paddleWidth, paddleHeight);
}

bool Bonus::isWithinPaddleBounds(float paddleX, float paddleY, float paddleWidth, float paddleHeight) const {
    return (x >= paddleX && x <= paddleX + paddleWidth &&
            y >= paddleY && y <= paddleY + paddleHeight);
}

PaddleSizeIncreaseBonus::PaddleSizeIncreaseBonus(float x, float y) 
    : Bonus(x, y, {0, 255, 0, 255}) {} 

PaddleSizeDecreaseBonus::PaddleSizeDecreaseBonus(float x, float y) 
    : Bonus(x, y, {0, 255, 0, 255}) {} 

BallSpeedIncreaseBonus::BallSpeedIncreaseBonus(float x, float y) 
    : Bonus(x, y, {255, 165, 0, 255}) {} 

BallSpeedDecreaseBonus::BallSpeedDecreaseBonus(float x, float y) 
    : Bonus(x, y, {255, 165, 0, 255}) {} 

StickyPaddleBonus::StickyPaddleBonus(float x, float y) 
    : Bonus(x, y, {128, 0, 128, 255}) {}

BottomPlatformBonus::BottomPlatformBonus(float x, float y) 
    : Bonus(x, y, {255, 255, 255, 255}) {} 

RandomTrajectoryBonus::RandomTrajectoryBonus(float x, float y) 
    : Bonus(x, y, {255, 192, 203, 255}) {} 

float PaddleSizeIncreaseBonus::calculateNewWidth(float currentWidth) const {
    return currentWidth * 1.5f;
}

void PaddleSizeIncreaseBonus::logWidthChange(float oldWidth, float newWidth) const {
    std::cout << "[DEBUG:Bonus.cpp] Paddle width increased from " << oldWidth << " to " << newWidth << std::endl;
}

void PaddleSizeIncreaseBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying PaddleSizeIncreaseBonus" << std::endl;
    float currentWidth = game->getPaddleWidth();
    float newWidth = calculateNewWidth(currentWidth);
    game->setPaddleWidth(newWidth);
    logWidthChange(currentWidth, newWidth);
}

float PaddleSizeDecreaseBonus::calculateNewWidth(float currentWidth) const {
    return currentWidth * 0.75f; 
}

void PaddleSizeDecreaseBonus::logWidthChange(float oldWidth, float newWidth) const {
    std::cout << "[DEBUG:Bonus.cpp] Paddle width decreased from " << oldWidth << " to " << newWidth << std::endl;
}

void PaddleSizeDecreaseBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying PaddleSizeDecreaseBonus" << std::endl;
    float currentWidth = game->getPaddleWidth();
    float newWidth = calculateNewWidth(currentWidth);
    game->setPaddleWidth(newWidth);
    logWidthChange(currentWidth, newWidth);
}

float BallSpeedIncreaseBonus::calculateNewSpeed(float currentSpeed) const {
    return currentSpeed * 1.5f;
}

void BallSpeedIncreaseBonus::normalizeAndApplySpeed(Game* game, float newSpeed) const {
    float speedX = game->getBallSpeedX();
    float speedY = game->getBallSpeedY();
    
    float magnitude = std::sqrt(speedX * speedX + speedY * speedY);
    if (magnitude > 0) {
        speedX = (speedX / magnitude) * newSpeed;
        speedY = (speedY / magnitude) * newSpeed;
    }
    
    game->setBallSpeed(speedX, speedY);
}

void BallSpeedIncreaseBonus::logSpeedChange(float oldSpeed, float newSpeed) const {
    std::cout << "[DEBUG:Bonus.cpp] Ball speed increased from " << oldSpeed << " to " << newSpeed << std::endl;
}

void BallSpeedIncreaseBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying BallSpeedIncreaseBonus" << std::endl;
    float currentSpeed = game->getBallSpeed();
    float newSpeed = calculateNewSpeed(currentSpeed);
    normalizeAndApplySpeed(game, newSpeed);
    logSpeedChange(currentSpeed, newSpeed);
}

float BallSpeedDecreaseBonus::calculateNewSpeed(float currentSpeed) const {
    return currentSpeed * 0.75f;
}

void BallSpeedDecreaseBonus::normalizeAndApplySpeed(Game* game, float newSpeed) const {
    float speedX = game->getBallSpeedX();
    float speedY = game->getBallSpeedY();
    
    float magnitude = std::sqrt(speedX * speedX + speedY * speedY);
    if (magnitude > 0) {
        speedX = (speedX / magnitude) * newSpeed;
        speedY = (speedY / magnitude) * newSpeed;
    }
    
    game->setBallSpeed(speedX, speedY);
}

void BallSpeedDecreaseBonus::logSpeedChange(float oldSpeed, float newSpeed) const {
    std::cout << "[DEBUG:Bonus.cpp] Ball speed decreased from " << oldSpeed << " to " << newSpeed << std::endl;
}

void BallSpeedDecreaseBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying BallSpeedDecreaseBonus" << std::endl;
    float currentSpeed = game->getBallSpeed();
    float newSpeed = calculateNewSpeed(currentSpeed);
    normalizeAndApplySpeed(game, newSpeed);
    logSpeedChange(currentSpeed, newSpeed);
}

void StickyPaddleBonus::logActivation(bool isSticky) const {
    std::cout << "[DEBUG:Bonus.cpp] Sticky paddle " << (isSticky ? "activated" : "deactivated") << std::endl;
}

void StickyPaddleBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying StickyPaddleBonus" << std::endl;
    bool currentState = game->isPaddleSticky();
    game->setStickyPaddle(!currentState);
    logActivation(!currentState);
}

void BottomPlatformBonus::logActivation() const {
    std::cout << "[DEBUG:Bonus.cpp] Bottom platform activated" << std::endl;
}

void BottomPlatformBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying BottomPlatformBonus" << std::endl;
    game->setBottomPlatform(true);
    logActivation();
}

float RandomTrajectoryBonus::generateRandomAngle() const {
    return (rand() % 90 - 45) * M_PI / 180.0f;
}

void RandomTrajectoryBonus::calculateAndApplyNewSpeed(Game* game, float currentSpeed, float randomAngle) const {
    float newSpeedX = currentSpeed * sin(randomAngle);
    float newSpeedY = -currentSpeed * cos(randomAngle); 
    game->setBallSpeed(newSpeedX, newSpeedY);
    logSpeedChange(currentSpeed, newSpeedX, newSpeedY);
}

void RandomTrajectoryBonus::logSpeedChange(float currentSpeed, float newSpeedX, float newSpeedY) const {
    std::cout << "[DEBUG:Bonus.cpp] Current ball speed: " << currentSpeed << std::endl;
    std::cout << "[DEBUG:Bonus.cpp] Random angle: " << (newSpeedX / currentSpeed) * 180.0f / M_PI << " degrees" << std::endl;
    std::cout << "[DEBUG:Bonus.cpp] New speed components - X: " << newSpeedX << ", Y: " << newSpeedY << std::endl;
}

void RandomTrajectoryBonus::applyBonus(Game* game) {
    std::cout << "[DEBUG:Bonus.cpp] Applying RandomTrajectoryBonus" << std::endl;
    float currentSpeed = game->getBallSpeed();
    float randomAngle = generateRandomAngle();
    calculateAndApplyNewSpeed(game, currentSpeed, randomAngle);
} 