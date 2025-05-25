#include "Board.hpp"
#include <cmath>
#define _USE_MATH_DEFINES
#include <iostream>

void Board::reset() {
    std::cout << "[Debug:Board.cpp] Board reset: old x=" << rect.x << ", new x=" << (Settings::WIDTH / 2 - rect.w / 2) << std::endl;
    rect.x = Settings::WIDTH / 2 - rect.w / 2;
    rect.y = Settings::HEIGHT - 10; 
}

void Board::handleLeftMovement() {
    rect.x -= speed;
}

void Board::handleRightMovement() {
    rect.x += speed;
}

void Board::enforceBoundaries() {
    if (rect.x < 0) {
        rect.x = 0;
    }
    if (rect.x > Settings::WIDTH - rect.w) {
        rect.x = Settings::WIDTH - rect.w;
    }
}

void Board::update(const Uint8* keystate) {
    if (keystate[SDL_SCANCODE_LEFT]) {
        handleLeftMovement();
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        handleRightMovement();
    }
    enforceBoundaries();
}

void Board::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

bool Board::checkBallCollision(Ball& ball) {
    int ballRadius = ball.getR();
    int ballX = ball.getX();
    int ballY = ball.getY();
    
    int ballBottom = ballY + ballRadius;
    int paddleTop = rect.y;

    return ballBottom >= paddleTop && ballBottom <= paddleTop + 5 &&
           ballX + ballRadius >= rect.x && ballX - ballRadius <= rect.x + rect.w &&
           ball.getSpeedY() > 0;
}