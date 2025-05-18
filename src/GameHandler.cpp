#include "GameHandler.hpp"

GameHandler::GameHandler(GameModel& model)
    : model(model)
{
    changeState(std::make_unique<WaitingForTouchState>(model.getTimings()));
}

void GameHandler::run() {
    const int FRAME_MS = 16;
    while (isRunning) {
        currentState->render(*this);
        handleEvents();
        update();
        SDL_Delay(FRAME_MS);
    }
}

void GameHandler::changeState(std::unique_ptr<GameState> newState) {
    currentState = std::move(newState);
}

void GameHandler::handleEvents() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            isRunning = false;
            break;
        }
        if (currentState) currentState->handleEvent(*this, ev);
    }
}

void GameHandler::update() {
    if (currentState) currentState->update(*this);
}
