#pragma once
#include "Settings.hpp"
#include "Bonus.hpp"

static constexpr int INF = std::numeric_limits<int>::max();

class Block {
public:
	Block(int x, int y, int width, int height, SDL_Color color, int health = 1);
	virtual ~Block() = default;
	
	virtual void render(SDL_Renderer* renderer);
	virtual bool isDestroyed() const;
	virtual void destroy();
	
	int getX() const { return rect.x; }
	int getY() const { return rect.y; }
	int getWidth() const { return rect.w; }
	int getHeight() const { return rect.h; }
	int getHealth() const { return health; }
	
	virtual bool checkCollision(int ballX, int ballY, int ballRadius) const;
	
	virtual int handleCollision(int damage);

protected:
	SDL_Rect rect;
	SDL_Color color;
	int health;
	bool destroyed;

private:
	int findClosestPoint(int value, int min, int max) const;
	bool isPointInRange(int point, int min, int max) const;
	int calculateDistanceSquared(int x1, int y1, int x2, int y2) const;
	
	bool isBlockDestroyed() const;
	int calculatePoints(int damage) const;
	void applyDamage(int damage);
};

class UsualBlock : public Block {
public:
	UsualBlock(int x, int y, int width, int height, SDL_Color color, int health = 1);
	int handleCollision(int damage) override;
};

class IndestructibleBlock : public Block {
public:
	IndestructibleBlock(int x, int y, int width, int height);
	int handleCollision(int damage) override;
};

class SpeedBlock : public Block {
public:
	SpeedBlock(int x, int y, int width, int height, SDL_Color color, int health = 1);
	int handleCollision(int damage) override;
};

class BonusBlock : public Block {
public:
	BonusBlock(int x, int y, int width, int height, SDL_Color color);
	bool handleBoardCollision() { return true; }
	int handleCollision(int damage) override;
	
	bool hasBonus() const { return bonusType.has_value(); }
	std::optional<BonusType> getBonusType() const { return bonusType; }
	void setBonusType(BonusType type) { bonusType = type; }

protected:
	std::optional<BonusType> bonusType;
};