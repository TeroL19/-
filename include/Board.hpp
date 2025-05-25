#pragma once
#include "Settings.hpp"
#include "Ball.hpp"

class Board {
public:
    Board(int width, int height, SDL_Color color, int speed = 7)
        : color(color), speed(speed) {
        startX = Settings::WIDTH / 2 - width / 2;
        startY = Settings::HEIGHT - 10;

        rect = { startX, startY, width, height };
    }

    void reset();

    void update(const Uint8* keystate);

    void render(SDL_Renderer* renderer);       

    bool checkBallCollision(Ball& ball);

    float getX() const { return rect.x; }
    float getY() const { return rect.y; }
    float getWidth() const { return rect.w; }
    float getHeight() const { return rect.h; }

    void setWidth(float newWidth) {
        float oldCenter = rect.x + rect.w / 2;
        rect.w = newWidth;
        rect.x = oldCenter - newWidth / 2;
    }
      

private:
    void handleLeftMovement();
    void handleRightMovement();
    void enforceBoundaries();

    SDL_Rect rect;    
    SDL_Color color;  
    int speed;

    int startX;
    int startY;
};  