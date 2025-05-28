#pragma once
#include "States.hpp"

class GameState;

class GameHandler {
public:
    explicit GameHandler(GameModel& model);
    void run();
    void changeState(std::unique_ptr<GameState> newState);

    GameModel& getModel() { return model; }

private:
    void handleEvents();
    void update();

    GameModel& model;
    bool isRunning = true;
    std::unique_ptr<GameState> currentState;
};
