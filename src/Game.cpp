#include "Game.hpp"
#include "Bonus.hpp"

Game::Game() : window(nullptr), renderer(nullptr), isRunning(true), isGameOver(false), isVictory(false), score(0), lives(Settings::MAX_LIVES),
        ball(Settings::WIDTH / 2, Settings::BALL_START_Y, Settings::BALL_RADIUS, 0, 0, { 255,255,255 }),
        board(Settings::BOARD_WIDTH, Settings::BOARD_HEIGHT, Settings::BOARD_COLOR),
        blocksGrid(Settings::GRID_ROWS, Settings::GRID_COLS, Settings::BLOCK_WIDTH, Settings::BLOCK_HEIGHT, 
                  Settings::GRID_START_X, Settings::GRID_START_Y, this),
        launchAngle(Settings::LAUNCH_ANGLE), showArrow(true),
        hasBottomPlatform(false),
        paddleSticky(false),
        originalPaddleWidth(Settings::ORIGINAL_PADDLE_WIDTH),
        originalBallSpeed(Settings::ORIGINAL_BALL_SPEED),
        stickyStartTime(0),
        stickyRelativeX(0) {

        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        this->window = SDL_CreateWindow(
            "Arkanoid",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            Settings::WIDTH, Settings::HEIGHT,
            SDL_WINDOW_SHOWN
        );

        this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

        this->font = TTF_OpenFont(Settings::fontPath.c_str(), 24);
        if (!this->font) {
            std::cout << "[DEBUG:Game.cpp] Failed to load font from path: " << Settings::fontPath << std::endl;
        } else {
            std::cout << "[DEBUG:Game.cpp] Font loaded successfully from: " << Settings::fontPath << std::endl;
        }

        this->lives = Settings::MAX_LIVES;
    }

Game::~Game() {
    if (this->font) {
        TTF_CloseFont(this->font);
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        update();
        if (isRunning) {
            render();
            SDL_Delay(16);
        }
    }
}

void Game::handleInput(const Uint8* keystate) {
    if (isGameOver) {
        handleGameOverInput(keystate);
        return;
    }

    board.update(keystate);

    handleBallLaunch(keystate);
    handlePaddleMovement(keystate);
}

void Game::handleGameOverInput(const Uint8* keystate) {
    if (keystate[SDL_SCANCODE_RETURN]) {
        SDL_Quit();
        exit(0);
    }
}

void Game::handleBallLaunch(const Uint8* keystate) {
    if (keystate[SDL_SCANCODE_RETURN] && ball.getState() == States::OnBoard && !paddleSticky) {
        const double LAUNCH_ANGLE = 75.0 * M_PI / 180.0;
        const double LAUNCH_SPEED = 7.0;

        double speedX = LAUNCH_SPEED * std::cos(LAUNCH_ANGLE);
        double speedY = -LAUNCH_SPEED * std::sin(LAUNCH_ANGLE);

        ball.setSpeedX(speedX);
        ball.setSpeedY(speedY);
        ball.setState(States::Running);
    }
}

void Game::handlePaddleMovement(const Uint8* keystate) {
    if (ball.getState() == States::OnBoard && !paddleSticky) {
        bool isLeftPressed = keystate[SDL_SCANCODE_LEFT];
        bool isRightPressed = keystate[SDL_SCANCODE_RIGHT];
        updateLaunchAngle(isLeftPressed, isRightPressed);
    }
}

void Game::updateLaunchAngle(bool isLeftPressed, bool isRightPressed) {
    if (isLeftPressed) {
        if (launchAngle > 90) {
            launchAngle = 90;
        }
        else {
            launchAngle = std::max(10.f, launchAngle - 2.f);
        }
    }
    if (isRightPressed) {
        if (launchAngle < 90) {
            launchAngle = 90;
        }
        else {
            launchAngle = std::min(170.f, launchAngle + 2.f);
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
}

void Game::update() {
    if (isGameOver) {
        return;
    }

    updateBallState();
    handleBallCollisions();
    handleBallFalling();
    checkVictoryCondition();
}

void Game::updateBallState() {
    if (ball.getState() == States::Running) {
        ball.updatePosition(Settings::WIDTH, Settings::HEIGHT);
        handleStickyBall();
    } else if (ball.getState() == States::OnBoard) {
        if (paddleSticky) {
            handleStickyBall();
        } else {
            ball.setPosition(board.getX() + board.getWidth() / 2, board.getY() - ball.getR());
        }
    }
}

void Game::handleStickyBall() {
    if (paddleSticky && ball.getState() == States::Running && 
        ball.getY() + ball.getR() >= board.getY() && 
        ball.getY() - ball.getR() <= board.getY() + board.getHeight() &&
        ball.getX() >= board.getX() && ball.getX() <= board.getX() + board.getWidth()) {
        
        ball.setState(States::OnBoard);
        ball.setSpeedX(0);
        ball.setSpeedY(0);
        stickyStartTime = SDL_GetTicks();
        stickyRelativeX = ball.getX() - board.getX();
    } else if (ball.getState() == States::OnBoard && paddleSticky) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - stickyStartTime >= 1000) { 
            ball.setState(States::Running);
            float relativeIntersectX = (ball.getX() - (board.getX() + board.getWidth() / 2)) / (board.getWidth() / 2);
            float bounceAngle = relativeIntersectX * 60.0f; 
            float angleRad = bounceAngle * M_PI / 180.0f;
            float speed = 7.0f; 
            ball.setSpeedX(speed * sin(angleRad));
            ball.setSpeedY(-speed * cos(angleRad));
        } else {
            ball.setPosition(board.getX() + stickyRelativeX, board.getY() - ball.getR());
        }
    } else {
        ball.handleCollision(board);
    }
}

void Game::handleBallCollisions() {
    blocksGrid.updateBonuses();
    blocksGrid.checkBonusCollision(board.getX(), board.getY(), board.getWidth(), board.getHeight());

    int newSpeedX = ball.getSpeedX();
    int newSpeedY = ball.getSpeedY();
    if (blocksGrid.checkCollision(ball.getX(), ball.getY(), ball.getR(), newSpeedX, newSpeedY)) {
        ball.setSpeedX(newSpeedX);
        ball.setSpeedY(newSpeedY);
    }
}

void Game::handleBallFalling() {
    if (ball.getY() + ball.getR() >= Settings::HEIGHT) {
        if (hasBottomPlatform) {
            ball.setSpeedY(-ball.getSpeedY());
            hasBottomPlatform = false;
        } else {
            lives--;
            std::cout << "[DEBUG:Game.cpp] Lives left: " << lives << std::endl;
            
            if (lives <= 0) {
                std::cout << "[DEBUG:Game.cpp] Game Over!" << std::endl;
                isGameOver = true;
                return;
            }
            
            paddleSticky = false;
            stickyStartTime = 0;
            stickyRelativeX = 0;
            
            ball.setState(States::OnBoard);
            ball.reset();
            board.reset();
            launchAngle = 90;
        }
    }
}

void Game::checkVictoryCondition() {
    if (blocksGrid.allDestructibleBlocksDestroyed()) {
        std::cout << "[DEBUG:Game.cpp] Victory! All destructible blocks destroyed!" << std::endl;
        isVictory = true;
        isGameOver = true;
    }
}

void Game::render() {
    if (isGameOver) {
        renderFinalScreen();
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderLives();
    renderScore();
    renderGameObjects();

    SDL_RenderPresent(renderer);
}

void Game::renderLives() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < Settings::MAX_LIVES; i++) {
        SDL_Rect lifeRect = {
            10 + i * (Settings::LIFE_SIZE + 5),
            Settings::LIVES_DISPLAY_Y - Settings::LIFE_SIZE,
            Settings::LIFE_SIZE,
            Settings::LIFE_SIZE
        };
        if (i < lives) {
            SDL_RenderFillRect(renderer, &lifeRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &lifeRect);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
    }
}

void Game::renderScore() {
    SDL_Color textColor = { 255, 255, 255, 255 };
    std::string scoreText = "Score: " + std::to_string(score);
    renderText(scoreText.c_str(), textColor, Settings::WIDTH - 150, Settings::LIVES_DISPLAY_Y - 20);
}

void Game::renderGameObjects() {
    blocksGrid.render(renderer);
    blocksGrid.renderBonuses(renderer);
    ball.render(renderer);
    board.render(renderer);
}

void Game::renderText(const char* text, SDL_Color color, int x, int y) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                x,
                y,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void Game::renderFinalScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const char* titleText = isVictory ? "VICTORY" : "GAME OVER";
    SDL_Color titleColor = isVictory ? SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255};

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, titleText, titleColor);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, ("Final Score: " + std::to_string(score)).c_str(), { 255, 255, 255, 255 });
    SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "Press ENTER to exit", { 200, 200, 200, 255 });

    if (titleSurface && scoreSurface && exitSurface) {
        int titleX = (Settings::WIDTH - titleSurface->w) / 2;
        int scoreX = (Settings::WIDTH - scoreSurface->w) / 2;
        int exitX = (Settings::WIDTH - exitSurface->w) / 2;

        int spacing = 30;
        int totalHeight = titleSurface->h + scoreSurface->h + exitSurface->h + spacing * 2;
        int startY = (Settings::HEIGHT - totalHeight) / 2;

        renderText(titleText, titleColor, titleX, startY);
        renderText(("Final Score: " + std::to_string(score)).c_str(), { 255, 255, 255, 255 }, scoreX, startY + titleSurface->h + spacing);
        renderText("Press ENTER to exit", { 200, 200, 200, 255 }, exitX, startY + titleSurface->h + scoreSurface->h + spacing * 2);
    }

    if (titleSurface) SDL_FreeSurface(titleSurface);
    if (scoreSurface) SDL_FreeSurface(scoreSurface);
    if (exitSurface) SDL_FreeSurface(exitSurface);

    SDL_RenderPresent(renderer);
}

void Game::resetGame() {
    paddleSticky = false;
    stickyStartTime = 0;
    stickyRelativeX = 0;
    hasBottomPlatform = false;
    showArrow = true;
    launchAngle = 90;
    
    ball.setState(States::OnBoard);
    ball.reset();
    ball.setSpeedX(0);
    ball.setSpeedY(0);
    
    board.reset();
    board.setWidth(originalPaddleWidth);
    
    lives = Settings::MAX_LIVES;
} 