#include "States.hpp"
#include "GameHandler.hpp"

void GameState::render(GameHandler& handler, int delay) {
    auto& model = handler.getModel();
    SDL_SetRenderDrawColor(model.getRenderer(), 30, 30, 30, 255);
    SDL_RenderClear(model.getRenderer());
    model.getGrid().render(model.getRenderer());
    SDL_RenderPresent(model.getRenderer());
    SDL_Delay(delay);
}

WaitingForTouchState::WaitingForTouchState(const AnimTimings& timings)
    : timings(timings) {}

void WaitingForTouchState::handleEvent(GameHandler& handler, SDL_Event& event) {
    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
        return;

    auto& model = handler.getModel();
    int mx = event.button.x, my = event.button.y;
    GameGrid& grid = model.getGrid();
    auto [gx, gy] = grid.pixelToGrid(mx, my);

    if (!clickInsideGrid(grid, gx, gy)) return;

    if (!firstSel) { firstSel = { gx, gy }; return; }

    std::pair<int, int> second = { gx, gy };
    if (*firstSel == second) { firstSel.reset(); return; }

    if (!isAdjacent(*firstSel, second)) {
        firstSel = second;
        return;
    }

    trySwapAndContinue(handler, second);
}

void WaitingForTouchState::update(GameHandler& handler) {
}

bool WaitingForTouchState::clickInsideGrid(GameGrid& grid, int x, int y) const {
    auto [w, h] = grid.getSize();
    return 0 <= x && 0 <= y && x < w && y < h;
}

bool WaitingForTouchState::isAdjacent(std::pair<int, int> a, std::pair<int, int> b) const {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second) == 1;
}

void WaitingForTouchState::trySwapAndContinue(GameHandler& handler, std::pair<int, int> second) {
    auto& model = handler.getModel();
    GameGrid& grid = model.getGrid();
    bool ok = grid.swapGems(firstSel->first, firstSel->second, second.first, second.second);
    if (ok) {
        render(handler, timings.click);
        handler.changeState(std::make_unique<ProcessMatchesState>(timings));
    }
    else {
        firstSel.reset();
    }
}

ProcessMatchesState::ProcessMatchesState(const AnimTimings& timings)
    : timings(timings) {}

void ProcessMatchesState::handleEvent(GameHandler&, SDL_Event&) {}

void ProcessMatchesState::update(GameHandler& handler) {
    handleMatches(handler);
    applyFalls(handler);

    auto& model = handler.getModel();
    if (model.getGrid().checkMatches())
        handler.changeState(std::make_unique<ProcessMatchesState>(timings));
    else
        handler.changeState(std::make_unique<WaitingForTouchState>(timings));
}

void ProcessMatchesState::handleMatches(GameHandler& handler) {
    auto& model = handler.getModel();
    auto& grid = model.getGrid();
    auto matches = grid.findMatches();
    if (matches.empty()) return;

    grid.removeMatches(model.getRenderer(), matches);
    render(handler, timings.match);
}

void ProcessMatchesState::applyFalls(GameHandler& handler) {
    auto& model = handler.getModel();
    auto& grid = model.getGrid();
    while (grid.applyGravity())
        render(handler, timings.fallStep);
}
