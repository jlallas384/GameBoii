#define SDL_MAIN_HANDLED

#include <memory>
#include "sdl_lcd.h"

#include "timer.h"
#include <iostream>
#define private public
#include "game_boy.h"
#undef private
std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
int main() {
    SDL_SetMainReady();
    SDL_Window* window = nullptr;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("gbmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);
    std::unique_ptr<SDLLCD> lcd = std::make_unique<SDLLCD>(window);
    GameBoy gb(std::move(lcd));
    gb.loadCartridge("./tetris.gb");
    gb.run();
    SDL_Event e;
    bool quit = false;
    begin = std::chrono::steady_clock::now();
    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
        gb.run();
    }

}