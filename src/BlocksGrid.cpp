#include "BlocksGrid.hpp"
#include "Game.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <random>
#include <iostream>

BlocksGrid::BlocksGrid(int rows, int cols, int blockWidth, int blockHeight, int startX, int startY, Game* game)
    : rows(rows), cols(cols), blockWidth(blockWidth), blockHeight(blockHeight), score(0), game(game) {
    
    blocks.resize(rows);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);
    std::uniform_int_distribution<> healthDis(1, 3); 
    std::uniform_int_distribution<> indestructibleDis(0, 100); 
    std::uniform_int_distribution<> bonusDis(0, 100); 
    std::uniform_int_distribution<> colorDis(0, 255); 
    std::uniform_int_distribution<> bonusTypeDis(0, 6); 

    blockWidth = Settings::WIDTH / cols; 
    blockHeight = blockWidth * 0.25;

    for (int i = 0; i < rows; i++) {
        blocks[i].resize(cols);
        for (int j = 0; j < cols; j++) {
            int randomValue = dis(gen);
            int health = healthDis(gen);
            int indestructibleChance = indestructibleDis(gen);
            int bonusChance = bonusDis(gen);
            
            SDL_Color randomColor = {
                static_cast<Uint8>(colorDis(gen)),
                static_cast<Uint8>(colorDis(gen)),
                static_cast<Uint8>(colorDis(gen)),
                255
            };
            
            if (i == 0 || indestructibleChance < 5) {
                blocks[i][j] = std::make_unique<IndestructibleBlock>(
                    j * blockWidth,
                    startY + i * blockHeight,
                    blockWidth - 1,
                    blockHeight - 1
                );
            } else if (bonusChance < 25) {
                blocks[i][j] = std::make_unique<BonusBlock>(
                    j * blockWidth,
                    startY + i * blockHeight,
                    blockWidth - 1,
                    blockHeight - 1,
                    randomColor
                );
                if (bonusDis(gen) < 100) {
                    auto bonusBlock = dynamic_cast<BonusBlock*>(blocks[i][j].get());
                    if (bonusBlock) {
                        bonusBlock->setBonusType(static_cast<BonusType>(bonusTypeDis(gen)));
                    }
                }
            } else if (randomValue < 35) {
                blocks[i][j] = std::make_unique<SpeedBlock>(
                    j * blockWidth,
                    startY + i * blockHeight,
                    blockWidth - 1,
                    blockHeight - 1,
                    randomColor,
                    health
                );
            } else {
                blocks[i][j] = std::make_unique<UsualBlock>(
                    j * blockWidth,
                    startY + i * blockHeight,
                    blockWidth - 1,
                    blockHeight - 1,
                    randomColor,
                    health
                );
            }
        }
    }
}

void BlocksGrid::render(SDL_Renderer* renderer) {
    for (auto& row : blocks) {
        for (auto& block : row) {
            if (block) {
                block->render(renderer);
            }
        }
    }
}

bool BlocksGrid::isBallInBlockBounds(int ballX, int ballY, int ballRadius, int blockLeft, int blockRight, int blockTop, int blockBottom) const {
    return ballX >= blockLeft && ballX <= blockRight &&
           ballY >= blockTop && ballY <= blockBottom;
}

void BlocksGrid::calculateBlockOverlap(int ballX, int ballY, int blockLeft, int blockRight, int blockTop, int blockBottom,
                                     int& overlapLeft, int& overlapRight, int& overlapTop, int& overlapBottom) const {
    overlapLeft = ballX - blockLeft;
    overlapRight = blockRight - ballX;
    overlapTop = ballY - blockTop;
    overlapBottom = blockBottom - ballY;
}

void BlocksGrid::adjustBallSpeed(double currentSpeed, int& newSpeedX, int& newSpeedY) const {
    const double MIN_SPEED = 5.0;
    const double MAX_SPEED = 7.0;
    double finalSpeed = std::sqrt(newSpeedX * newSpeedX + newSpeedY * newSpeedY);
    
    if (finalSpeed < MIN_SPEED) {
        double speedRatio = MIN_SPEED / finalSpeed;
        newSpeedX *= speedRatio;
        newSpeedY *= speedRatio;
    } else if (finalSpeed > MAX_SPEED) {
        double speedRatio = MAX_SPEED / finalSpeed;
        newSpeedX *= speedRatio;
        newSpeedY *= speedRatio;
    }

    newSpeedX *= 1.1f;
    newSpeedY *= 1.1f;
}

void BlocksGrid::handleSpeedBlockCollision(int& newSpeedX, int& newSpeedY) const {
    const double MAX_SPEED = 7.0;
    double speedMultiplier = 1.5;
    newSpeedX *= speedMultiplier;
    newSpeedY *= speedMultiplier;
    
    double newSpeed = std::sqrt(newSpeedX * newSpeedX + newSpeedY * newSpeedY);
    if (newSpeed > MAX_SPEED) {
        double speedRatio = MAX_SPEED / newSpeed;
        newSpeedX *= speedRatio;
        newSpeedY *= speedRatio;
    }
}

void BlocksGrid::handleBonusBlockCollision(Block* block) {
    if (auto* bonusBlock = dynamic_cast<BonusBlock*>(block)) {
        if (bonusBlock->hasBonus()) {
            auto bonusType = bonusBlock->getBonusType();
            if (bonusType) {
                createBonus(bonusBlock->getX() + bonusBlock->getWidth() / 2,
                          bonusBlock->getY() + bonusBlock->getHeight(),
                          *bonusType);
            }
        }
    }
}

void BlocksGrid::processBlockCollision(Block* block, int& points) {
    points = block->handleCollision(1); 
    score += points;
    if (game) {
        game->addScore(points);
    }
    std::cout << "[DEBUG:BlocksGrid.cpp] Block hit! Points: " << points << ", Total score: " << score << std::endl;
}

bool BlocksGrid::checkCollision(int ballX, int ballY, int ballRadius, int& newSpeedX, int& newSpeedY) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (!blocks[i][j] || blocks[i][j]->isDestroyed()) continue;
            
            int blockLeft = blocks[i][j]->getX();
            int blockRight = blockLeft + blocks[i][j]->getWidth();
            int blockTop = blocks[i][j]->getY();
            int blockBottom = blockTop + blocks[i][j]->getHeight();
            
            blockLeft -= ballRadius;
            blockRight += ballRadius;
            blockTop -= ballRadius;
            blockBottom += ballRadius;
            
            if (isBallInBlockBounds(ballX, ballY, ballRadius, blockLeft, blockRight, blockTop, blockBottom)) {
                int overlapLeft, overlapRight, overlapTop, overlapBottom;
                calculateBlockOverlap(ballX, ballY, blockLeft, blockRight, blockTop, blockBottom,
                                    overlapLeft, overlapRight, overlapTop, overlapBottom);
                
                int minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
                double currentSpeed = std::sqrt(newSpeedX * newSpeedX + newSpeedY * newSpeedY);
                
                if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                    newSpeedX = -newSpeedX;
                }
                if (minOverlap == overlapTop || minOverlap == overlapBottom) {
                    newSpeedY = -newSpeedY;
                }
                
                adjustBallSpeed(currentSpeed, newSpeedX, newSpeedY);
                
                int points = 0;
                processBlockCollision(blocks[i][j].get(), points);
                
                if (dynamic_cast<SpeedBlock*>(blocks[i][j].get())) {
                    handleSpeedBlockCollision(newSpeedX, newSpeedY);
                }
                
                handleBonusBlockCollision(blocks[i][j].get());
                
                return true;
            }
        }
    }
    return false;
}

void BlocksGrid::findClosestPoint(int ballX, int ballY, int blockX, int blockY, int blockWidth, int blockHeight,
                                int& closestX, int& closestY) const {
    closestX = std::max(blockX, std::min(ballX, blockX + blockWidth));
    closestY = std::max(blockY, std::min(ballY, blockY + blockHeight));
}

void BlocksGrid::calculateBounceVector(float dx, float dy, float distance, int& newSpeedX, int& newSpeedY) const {
    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }
    
    float dot = newSpeedX * dx + newSpeedY * dy;
    newSpeedX = newSpeedX - 2 * dot * dx;
    newSpeedY = newSpeedY - 2 * dot * dy;
}

void BlocksGrid::handleCornerCollision(float dx, float dy, float distance, int& newSpeedX, int& newSpeedY) const {
    calculateBounceVector(dx, dy, distance, newSpeedX, newSpeedY);
    
    const float CORNER_IMPULSE = 1.5f;
    newSpeedX *= CORNER_IMPULSE;
    newSpeedY *= CORNER_IMPULSE;
}

void BlocksGrid::handleSideCollision(bool hitFromLeft, bool hitFromTop, int& newSpeedX, int& newSpeedY) const {
    if (hitFromLeft || !hitFromLeft) {
        newSpeedX = -newSpeedX;
        newSpeedX *= 1.2f;
    }
    if (hitFromTop || !hitFromTop) {
        newSpeedY = -newSpeedY;
        newSpeedY *= 1.2f;
    }
}

void BlocksGrid::adjustSpeedForMinimumAngle(int& newSpeedX, int& newSpeedY) const {
    const float MIN_SPEED = 7.0f;
    const float MIN_ANGLE = 30.0f;
    float angle = std::abs(std::atan2(newSpeedY, newSpeedX) * 180.0f / M_PI);
    
    if (angle < MIN_ANGLE || angle > 180.0f - MIN_ANGLE) {
        float sign = (newSpeedX > 0) ? 1.0f : -1.0f;
        newSpeedX = sign * MIN_SPEED * std::cos(MIN_ANGLE * M_PI / 180.0f);
        newSpeedY = -MIN_SPEED * std::sin(MIN_ANGLE * M_PI / 180.0f);
    }
}

void BlocksGrid::addRandomDeviation(int& newSpeedX, int& newSpeedY) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-0.1f, 0.1f);
    newSpeedX += dis(gen);
    newSpeedY += dis(gen);
}

void BlocksGrid::normalizeSpeed(int& newSpeedX, int& newSpeedY) const {
    const float MIN_SPEED = 7.0f;
    float finalSpeed = std::sqrt(newSpeedX * newSpeedX + newSpeedY * newSpeedY);
    if (finalSpeed > 0) {
        newSpeedX = (newSpeedX / finalSpeed) * MIN_SPEED;
        newSpeedY = (newSpeedY / finalSpeed) * MIN_SPEED;
    }
}

void BlocksGrid::determineBounceDirection(int ballX, int ballY, int blockX, int blockY,
                                        int blockWidth, int blockHeight,
                                        int ballRadius,
                                        int& newSpeedX, int& newSpeedY) {
    int closestX, closestY;
    findClosestPoint(ballX, ballY, blockX, blockY, blockWidth, blockHeight, closestX, closestY);
    
    float dx = ballX - closestX;
    float dy = ballY - closestY;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    bool hitFromLeft = std::abs(ballX - blockX) < std::abs(ballX - (blockX + blockWidth));
    bool hitFromTop = std::abs(ballY - blockY) < std::abs(ballY - (blockY + blockHeight));
    bool hitCorner = std::abs(ballX - closestX) < ballRadius && std::abs(ballY - closestY) < ballRadius;
    
    if (hitCorner) {
        handleCornerCollision(dx, dy, distance, newSpeedX, newSpeedY);
    } else {
        handleSideCollision(hitFromLeft, hitFromTop, newSpeedX, newSpeedY);
    }
    
    adjustSpeedForMinimumAngle(newSpeedX, newSpeedY);
    addRandomDeviation(newSpeedX, newSpeedY);
    normalizeSpeed(newSpeedX, newSpeedY);
}

bool BlocksGrid::allBlocksDestroyed() const {
    for (const auto& row : blocks) {
        for (const auto& block : row) {
            if (block && !block->isDestroyed()) {
                return false;
            }
        }
    }
    return true;
}

void BlocksGrid::updateBonuses() {
    for (auto& bonus : activeBonuses) {
        bonus->update();
    }
    removeInactiveBonuses();
}

void BlocksGrid::renderBonuses(SDL_Renderer* renderer) {
    for (auto& bonus : activeBonuses) {
        bonus->render(renderer);
    }
}

bool BlocksGrid::checkBonusCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight) {
    for (auto it = activeBonuses.begin(); it != activeBonuses.end();) {
        if ((*it)->checkCollision(paddleX, paddleY, paddleWidth, paddleHeight)) {
            processBonusCollision(*it, paddleX, paddleY, paddleWidth, paddleHeight);
            it = activeBonuses.erase(it);
            return true;
        } else {
            ++it;
        }
    }
    return false;
}

BonusType BlocksGrid::getRandomBonusType() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 6);

    switch (dis(gen)) {
        case 0: return BonusType::PADDLE_SIZE_INCREASE;
        case 1: return BonusType::PADDLE_SIZE_DECREASE;
        case 2: return BonusType::BALL_SPEED_INCREASE;
        case 3: return BonusType::BALL_SPEED_DECREASE;
        case 4: return BonusType::STICKY_PADDLE;
        case 5: return BonusType::BOTTOM_PLATFORM;
        case 6: return BonusType::RANDOM_TRAJECTORY;
        default: return BonusType::PADDLE_SIZE_INCREASE;
    }
}

void BlocksGrid::createBonus(float x, float y, BonusType type) {
    std::unique_ptr<Bonus> bonus;
    switch (type) {
        case BonusType::PADDLE_SIZE_INCREASE:
            bonus = std::make_unique<PaddleSizeIncreaseBonus>(x, y);
            break;
        case BonusType::PADDLE_SIZE_DECREASE:
            bonus = std::make_unique<PaddleSizeDecreaseBonus>(x, y);
            break;
        case BonusType::BALL_SPEED_INCREASE:
            bonus = std::make_unique<BallSpeedIncreaseBonus>(x, y);
            break;
        case BonusType::BALL_SPEED_DECREASE:
            bonus = std::make_unique<BallSpeedDecreaseBonus>(x, y);
            break;
        case BonusType::STICKY_PADDLE:
            bonus = std::make_unique<StickyPaddleBonus>(x, y);
            break;
        case BonusType::BOTTOM_PLATFORM:
            bonus = std::make_unique<BottomPlatformBonus>(x, y);
            break;
        case BonusType::RANDOM_TRAJECTORY:
            bonus = std::make_unique<RandomTrajectoryBonus>(x, y);
            break;
    }
    if (bonus) {
        activeBonuses.push_back(std::move(bonus));
    }
}

bool BlocksGrid::allDestructibleBlocksDestroyed() const {
    for (const auto& row : blocks) {
        for (const auto& block : row) {
            if (block && !block->isDestroyed()) {
                if (dynamic_cast<const IndestructibleBlock*>(block.get()) == nullptr) {
                    return false; 
                }
            }
        }
    }
    return true; 
}

void BlocksGrid::removeInactiveBonuses() {
    for (auto it = activeBonuses.begin(); it != activeBonuses.end();) {
        if (!(*it)->isActive()) {
            it = activeBonuses.erase(it);
        } else {
            ++it;
        }
    }
}

void BlocksGrid::processBonusCollision(std::unique_ptr<Bonus>& bonus, float paddleX, float paddleY, float paddleWidth, float paddleHeight) {
    if (bonus->checkCollision(paddleX, paddleY, paddleWidth, paddleHeight)) {
        bonus->applyBonus(game);
        std::cout << "[DEBUG:BlocksGrid.cpp] Bonus applied and removed" << std::endl;
    }
} 