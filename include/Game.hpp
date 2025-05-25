#pragma once
#include "Settings.hpp"
#include "Ball.hpp"
#include "Board.hpp"
#include "BlocksGrid.hpp"
#include "Bonus.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <string>

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Ball ball;
    Board board;
    BlocksGrid blocksGrid;
    float launchAngle;
    bool showArrow;
    bool isRunning;
    bool isGameOver;
    bool isVictory;
    int score;
    int lives;
    static const int MAX_LIVES = 3;
    static const int LIVES_DISPLAY_HEIGHT = 2;
    static const int LIVES_DISPLAY_Y = 30;
    static const int LIFE_SIZE = 20;

    bool hasBottomPlatform;
    bool paddleSticky;
    float originalPaddleWidth;
    float originalBallSpeed;
    Uint32 stickyStartTime;
    static const Uint32 STICKY_HOLD_TIME = 1000;
    float stickyRelativeX;

    void handleGameOverInput(const Uint8* keystate);
    void handleBallLaunch(const Uint8* keystate);
    void handlePaddleMovement(const Uint8* keystate);
    void updateLaunchAngle(bool isLeftPressed, bool isRightPressed);

    void updateBallState();
    void handleStickyBall();
    void handleBallCollisions();
    void handleBallFalling();
    void checkVictoryCondition();

    void renderLives();
    void renderScore();
    void renderGameObjects();
    void renderFinalScreen();
    void renderText(const char* text, SDL_Color color, int x, int y);

public:
    Game();
    ~Game();

    void run();
    void handleInput(const Uint8* keystate);
    void handleEvents();
    void update();
    void render();
    void resetGame();
    void addScore(int points) { score += points; }

    float getBallSpeed() const {
        return std::sqrt(ball.getSpeedX() * ball.getSpeedX() + 
                        ball.getSpeedY() * ball.getSpeedY());
    }
    
    void setBallSpeed(float speedX, float speedY) {
        ball.setSpeedX(speedX);
        ball.setSpeedY(speedY);
    }

    float getPaddleWidth() const {
        return board.getWidth();
    }

    void setPaddleWidth(float width) {
        board.setWidth(width);
    }

    float getBallSpeedX() const {
        return ball.getSpeedX();
    }

    float getBallSpeedY() const {
        return ball.getSpeedY();
    }

    void setStickyPaddle(bool sticky) {
        paddleSticky = sticky;
    }

    void setBottomPlatform(bool hasPlatform) {
        hasBottomPlatform = hasPlatform;
    }

    bool isPaddleSticky() const {
        return paddleSticky;
    }
}; 