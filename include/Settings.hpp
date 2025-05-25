#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>
#include <random>
#include <optional>
#include <filesystem>

namespace Settings {
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr int FPS = 60;
    constexpr int FRAME_DELAY = 1000 / FPS;

    constexpr int BALL_RADIUS = 10;
    constexpr float BALL_SPEED = 7.0f;
    constexpr int BALL_START_Y = HEIGHT - 50;

    constexpr int BOARD_WIDTH = 100;
    constexpr int BOARD_HEIGHT = 20;
    constexpr SDL_Color BOARD_COLOR = {0, 255, 0, 255};

    constexpr int GRID_ROWS = 7;
    constexpr int GRID_COLS = 8;
    constexpr int GRID_START_X = 50;
    constexpr int GRID_START_Y = 50;
    constexpr int BLOCK_WIDTH = 60;
    constexpr int BLOCK_HEIGHT = 30;

    constexpr int MAX_LIVES = 3;
    constexpr int LAUNCH_ANGLE = 90;
    constexpr float ORIGINAL_BALL_SPEED = 7.0f;
    constexpr int ORIGINAL_PADDLE_WIDTH = 100;

    constexpr int LIFE_SIZE = 20;
    constexpr int LIVES_DISPLAY_Y = 30;

    static std::string fontPath = "C:/Windows/Fonts/arial.ttf";
}

enum class States {
    OnBoard,
    Running,
    Stuck
}; 