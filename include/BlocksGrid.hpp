#pragma once
#include "Block.hpp"
#include "Bonus.hpp"
#include "Settings.hpp"
#include <vector>
#include <memory>

class Game; 

class BlocksGrid {
public:
	BlocksGrid(int rows, int cols, int blockWidth, int blockHeight, int startX, int startY, Game* game);

	void render(SDL_Renderer* renderer);
	bool checkCollision(int ballX, int ballY, int ballRadius, int& newSpeedX, int& newSpeedY);
	bool allBlocksDestroyed() const;
	bool allDestructibleBlocksDestroyed() const;
	int getScore() const { return score; };

	void updateBonuses();
	void renderBonuses(SDL_Renderer* renderer);
	bool checkBonusCollision(float paddleX, float paddleY, float paddleWidth, float paddleHeight);

private:
	std::vector<std::vector<std::unique_ptr<Block>>> blocks;
	std::vector<std::unique_ptr<Bonus>> activeBonuses;
	int rows;
	int cols;
	int blockWidth;
	int blockHeight;
	int score;
	Game* game;

	bool isBallInBlockBounds(int ballX, int ballY, int ballRadius, int blockLeft, int blockRight, int blockTop, int blockBottom) const;
	void calculateBlockOverlap(int ballX, int ballY, int blockLeft, int blockRight, int blockTop, int blockBottom, 
							 int& overlapLeft, int& overlapRight, int& overlapTop, int& overlapBottom) const;
	void adjustBallSpeed(double currentSpeed, int& newSpeedX, int& newSpeedY) const;
	void handleSpeedBlockCollision(int& newSpeedX, int& newSpeedY) const;
	void handleBonusBlockCollision(Block* block);
	void processBlockCollision(Block* block, int& points);

	void findClosestPoint(int ballX, int ballY, int blockX, int blockY, int blockWidth, int blockHeight, 
						 int& closestX, int& closestY) const;
	void calculateBounceVector(float dx, float dy, float distance, int& newSpeedX, int& newSpeedY) const;
	void handleCornerCollision(float dx, float dy, float distance, int& newSpeedX, int& newSpeedY) const;
	void handleSideCollision(bool hitFromLeft, bool hitFromTop, int& newSpeedX, int& newSpeedY) const;
	void adjustSpeedForMinimumAngle(int& newSpeedX, int& newSpeedY) const;
	void addRandomDeviation(int& newSpeedX, int& newSpeedY) const;
	void normalizeSpeed(int& newSpeedX, int& newSpeedY) const;

	BonusType getRandomBonusType();
	void createBonus(float x, float y, BonusType type);
	void removeInactiveBonuses();
	void processBonusCollision(std::unique_ptr<Bonus>& bonus, float paddleX, float paddleY, float paddleWidth, float paddleHeight);

	void determineBounceDirection(int ballX, int ballY, int blockX, int blockY, 
								int blockWidth, int blockHeight, 
								int ballRadius,
								int& newSpeedX, int& newSpeedY);
};