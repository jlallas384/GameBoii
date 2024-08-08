#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "game_boy.h"

class Application {
public:
    Application();
    ~Application();
    void run();
private:
    void pollEvents();
    bool quit = false;
    SDL_Window* window = nullptr;
    std::unique_ptr<GameBoy> gameBoy;
};