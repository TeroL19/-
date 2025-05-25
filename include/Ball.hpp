#pragma once

#include "SDL.h"
#include "algorithm"
#include "Settings.hpp"

class Board;

class Ball {
public:
    Ball(int x, int y, int radius, int speedX, int speedY, SDL_Color color)
        : m_x(x), m_y(y), m_radius(radius), m_color(color) {
    state = States::OnBoard;

    m_speedX = speedX;
    m_speedY = speedY;
    isSticky = false;
    }

    void updatePosition(int screenWidth, int screenHeight);

    void reset();

    void render(SDL_Renderer* renderer);      

    bool handleCollision(Board& board);

    void setPosition(int x, int y);

    void setSpeedX(int speedX);

    void setSpeedY(int speedY);

    void setState(States newState);

    void setSticky(bool sticky) { isSticky = sticky; }

    int getX() const;

    int getY() const;

    int getSpeedX() const;

    int getSpeedY() const;

    int getR() const;

    States getState() const;

    bool isStickyBall() const { return isSticky; }

private:
    void handleWallCollision(int screenWidth, int screenHeight);
    void updateBallPosition();
    
    void drawCirclePoint(SDL_Renderer* renderer, int x, int y) const;
    bool isPointInCircle(int x, int y) const;
    
    bool isCollidingWithBoard(float boardX, float boardY, float boardWidth, float boardHeight) const;

    int m_x, m_y;          
    int m_radius;          
    int m_speedX, m_speedY; 
    SDL_Color m_color;

    States state;
    bool isSticky;
};