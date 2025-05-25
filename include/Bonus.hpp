#pragma once
#include "Settings.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <optional>

class Game;

enum class BonusType {
    PADDLE_SIZE_INCREASE,
    PADDLE_SIZE_DECREASE,
    BALL_SPEED_INCREASE,
    BALL_SPEED_DECREASE,
    STICKY_PADDLE,
    BOTTOM_PLATFORM,
    RANDOM_TRAJECTORY
};

class Bonus {
public:
    Bonus(float x, float y, SDL_Color color = {255, 255, 0, 255});
    virtual ~Bonus() = default;

    virtual void update();
    virtual void render(SDL_Renderer* renderer);
    bool isActive() const { return active; }
    virtual bool checkCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight);
    virtual void applyBonus(Game* game) = 0;
    virtual BonusType getType() const = 0;

    void deactivate() { active = false; }

    static const int SIZE = 20;

protected:
    float x, y;
    float speedY;
    bool active;
    SDL_Color color;

    void updatePosition();
    bool isOutOfBounds() const;
    void renderBonusRect(SDL_Renderer* renderer) const;
    bool isWithinPaddleBounds(float paddleX, float paddleY, float paddleWidth, float paddleHeight) const;
};

class PaddleSizeIncreaseBonus : public Bonus {
public:
    PaddleSizeIncreaseBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::PADDLE_SIZE_INCREASE; }

private:
    float calculateNewWidth(float currentWidth) const;
    void logWidthChange(float oldWidth, float newWidth) const;
};

class PaddleSizeDecreaseBonus : public Bonus {
public:
    PaddleSizeDecreaseBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::PADDLE_SIZE_DECREASE; }

private:
    float calculateNewWidth(float currentWidth) const;
    void logWidthChange(float oldWidth, float newWidth) const;
};

class BallSpeedIncreaseBonus : public Bonus {
public:
    BallSpeedIncreaseBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::BALL_SPEED_INCREASE; }

private:
    float calculateNewSpeed(float currentSpeed) const;
    void normalizeAndApplySpeed(Game* game, float newSpeed) const;
    void logSpeedChange(float oldSpeed, float newSpeed) const;
};

class BallSpeedDecreaseBonus : public Bonus {
public:
    BallSpeedDecreaseBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::BALL_SPEED_DECREASE; }

private:
    float calculateNewSpeed(float currentSpeed) const;
    void normalizeAndApplySpeed(Game* game, float newSpeed) const;
    void logSpeedChange(float oldSpeed, float newSpeed) const;
};

class StickyPaddleBonus : public Bonus {
public:
    StickyPaddleBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::STICKY_PADDLE; }

protected:
    void logActivation(bool isSticky) const;
};

class BottomPlatformBonus : public Bonus {
public:
    BottomPlatformBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::BOTTOM_PLATFORM; }

private:
    void logActivation() const;
};

class RandomTrajectoryBonus : public Bonus {
public:
    RandomTrajectoryBonus(float x, float y);
    void applyBonus(Game* game) override;
    BonusType getType() const override { return BonusType::RANDOM_TRAJECTORY; }

private:
    float generateRandomAngle() const;
    void calculateAndApplyNewSpeed(Game* game, float currentSpeed, float randomAngle) const;
    void logSpeedChange(float currentSpeed, float newSpeedX, float newSpeedY) const;
}; 