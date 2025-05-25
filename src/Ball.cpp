#include "Ball.hpp"
#include "Board.hpp"
#include <cmath>

void Ball::updateBallPosition() {
    m_x += m_speedX;
    m_y += m_speedY;
}

void Ball::handleWallCollision(int screenWidth, int screenHeight) {
    if (m_x - m_radius < 0) {
        m_x = m_radius;
        m_speedX = -m_speedX;
    }
    else if (m_x + m_radius > screenWidth) {
        m_x = screenWidth - m_radius;
        m_speedX = -m_speedX;
    }

    if (m_y - m_radius < 0) {
        m_y = m_radius;
        m_speedY = -m_speedY;
    }
}

void Ball::updatePosition(int screenWidth, int screenHeight) {
    if (state == States::Running) {
        updateBallPosition();
        handleWallCollision(screenWidth, screenHeight);
    }
}

void Ball::reset() {
    m_x = Settings::WIDTH / 2;
    m_y = Settings::HEIGHT - 50;
    m_speedX = 0;
    m_speedY = 0;
    state = States::OnBoard;
    isSticky = false;
}

bool Ball::isPointInCircle(int x, int y) const {
    return x * x + y * y <= m_radius * m_radius;
}

void Ball::drawCirclePoint(SDL_Renderer* renderer, int x, int y) const {
    SDL_RenderDrawPoint(renderer, m_x + x, m_y + y);
}

void Ball::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, m_color.r, m_color.g, m_color.b, m_color.a);
    
    for (int w = -m_radius; w <= m_radius; w++) {
        for (int h = -m_radius; h <= m_radius; h++) {
            if (isPointInCircle(w, h)) {
                drawCirclePoint(renderer, w, h);
            }
        }
    }
}

void Ball::setPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

void Ball::setSpeedX(int speedX) {
    m_speedX = speedX;
}

void Ball::setSpeedY(int speedY) {
    m_speedY = speedY;
}

void Ball::setState(States newState) {
    state = newState;
}

bool Ball::isCollidingWithBoard(float boardX, float boardY, float boardWidth, float boardHeight) const {
    return m_x + m_radius >= boardX && 
           m_x - m_radius <= boardX + boardWidth &&
           m_y + m_radius >= boardY && 
           m_y - m_radius <= boardY + boardHeight;
}

bool Ball::handleCollision(Board& board) {
    if (state != States::Running) return false;

    float boardX = board.getX();
    float boardY = board.getY();
    float boardWidth = board.getWidth();
    float boardHeight = board.getHeight();

    if (isCollidingWithBoard(boardX, boardY, boardWidth, boardHeight)) {
        if (isSticky) {
            state = States::OnBoard;
            m_speedX = 0;
            m_speedY = 0;
            return true;
        }

        float relativeIntersectX = (m_x - (boardX + boardWidth / 2)) / (boardWidth / 2);
        float bounceAngle = relativeIntersectX * 60.0f;
        float angleRad = bounceAngle * M_PI / 180.0f;
        float speed = std::sqrt(m_speedX * m_speedX + m_speedY * m_speedY);
        m_speedX = speed * sin(angleRad);
        m_speedY = -speed * cos(angleRad);
        m_y = boardY - m_radius;
        return true;
    }
    
    return false;
}

int Ball::getX() const {
    return m_x;
}

int Ball::getY() const {
    return m_y;
}

int Ball::getSpeedX() const {
    return m_speedX;
}

int Ball::getSpeedY() const {
    return m_speedY;
}

int Ball::getR() const {
    return m_radius;
}

States Ball::getState() const {
    return state;
}
