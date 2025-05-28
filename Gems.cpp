#include "GameHandler.hpp"

int wmain(int argc, char* argv[]) {
    GameModel model;

    if (!model.getWindow() || !model.getRenderer()) {
        return 1;
    }

    GameHandler game(model);
    game.run();

    return 0;
}
