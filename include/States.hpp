#pragma once
#include "GameGrid.hpp"
#include "GameModel.hpp"

class GameHandler;

class GameState {
public:
    virtual ~GameState() = default;
    virtual void handleEvent(GameHandler& handler, SDL_Event& event) = 0;
    virtual void update(GameHandler& handler) = 0;
    void render(GameHandler& handler, int delay = 100);
};

class WaitingForTouchState : public GameState {
public:
    explicit WaitingForTouchState(const AnimTimings& timings);
    void handleEvent(GameHandler& handler, SDL_Event& event) override;
    void update(GameHandler& handler) override;

private:
    std::optional<std::pair<int, int>> firstSel;
    const AnimTimings& timings;

    bool clickInsideGrid(GameGrid& grid, int x, int y) const;
    bool isAdjacent(std::pair<int, int> a, std::pair<int, int> b) const;
    void trySwapAndContinue(GameHandler& handler, std::pair<int, int> second);
};

class ProcessMatchesState : public GameState {
public:
    explicit ProcessMatchesState(const AnimTimings& timings);
    void handleEvent(GameHandler& handler, SDL_Event& event) override;
    void update(GameHandler& handler) override;
private:
    const AnimTimings& timings;

    void handleMatches(GameHandler& handler);
    void applyFalls(GameHandler& handler);
};
