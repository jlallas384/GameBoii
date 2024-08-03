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
    window = SDL_CreateWindow("gbmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 3, 144 * 3, SDL_WINDOW_SHOWN);


    std::unique_ptr<SDLLCD> lcd = std::make_unique<SDLLCD>(window);
    GameBoy gb(std::move(lcd));
    gb.loadCartridge("./tetris.gb");
    SDL_Event e;
    bool quit = false;
    begin = std::chrono::steady_clock::now();
    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        gb.changeDPadState(GameBoy::kLeft, true);
                        break;
                    case SDLK_DOWN:
                        gb.changeDPadState(GameBoy::kDown, true);
                        break;
                    case SDLK_RIGHT:
                        gb.changeDPadState(GameBoy::kRight, true);
                        break;
                    case SDLK_UP:
                        gb.changeDPadState(GameBoy::kUp, true);
                        break;
                    case SDLK_z:
                        gb.changeButtonState(GameBoy::kA, true);
                        break;
                    case SDLK_x:
                        gb.changeButtonState(GameBoy::kB, true);
                        break;
                    case SDLK_SPACE:
                        gb.changeButtonState(GameBoy::kStart, true);
                        break;
                }
            }
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        gb.changeDPadState(GameBoy::kLeft, false);
                        break;
                    case SDLK_DOWN:
                        gb.changeDPadState(GameBoy::kDown, false);
                        break;
                    case SDLK_RIGHT:
                        gb.changeDPadState(GameBoy::kRight, false);
                        break;
                    case SDLK_UP:
                        gb.changeDPadState(GameBoy::kUp, false);
                        break;
                    case SDLK_z:
                        gb.changeButtonState(GameBoy::kA, false);
                        break;
                    case SDLK_x:
                        gb.changeButtonState(GameBoy::kB, false);
                        break;
                    case SDLK_SPACE:
                        gb.changeButtonState(GameBoy::kStart, false);
                        break;
                }
            }
            if (e.type == SDL_QUIT) quit = true;
        }
        gb.run();
    }

}