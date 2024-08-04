#define SDL_MAIN_HANDLED

#include <memory>
#include "sdl_lcd.h"
#include "timer.h"
#include <iostream>
#include "game_boy.h"

std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
int main() {
    SDL_SetMainReady();
    SDL_Window* window = nullptr;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("gbmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 3, 144 * 3, SDL_WINDOW_SHOWN);


    std::unique_ptr<SDLLCD> lcd = std::make_unique<SDLLCD>(window);
    GameBoy gb(std::move(lcd));
    Joypad& joypad = gb.getJoypad();
    SDL_Event e;
    bool quit = false;
    begin = std::chrono::steady_clock::now();
    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_DROPFILE) {
                gb.loadCartridge(e.drop.file);
                SDL_free(e.drop.file);
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        joypad.press(DPadKind::kLeft);
                        break;
                    case SDLK_DOWN:
                        joypad.press(DPadKind::kDown);
                        break;
                    case SDLK_RIGHT:
                        joypad.press(DPadKind::kRight);
                        break;
                    case SDLK_UP:
                        joypad.press(DPadKind::kUp);
                        break;
                    case SDLK_z:
                        joypad.press(ButtonKind::kA);
                        break;
                    case SDLK_x:
                        joypad.press(ButtonKind::kB);
                        break;
                    case SDLK_SPACE:
                        joypad.press(ButtonKind::kStart);
                        break;
                }
            }
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        joypad.unpress(DPadKind::kLeft);
                        break;
                    case SDLK_DOWN:
                        joypad.unpress(DPadKind::kDown);
                        break;
                    case SDLK_RIGHT:
                        joypad.unpress(DPadKind::kRight);
                        break;
                    case SDLK_UP:
                        joypad.unpress(DPadKind::kUp);
                        break;
                    case SDLK_z:
                        joypad.unpress(ButtonKind::kA);
                        break;
                    case SDLK_x:
                        joypad.unpress(ButtonKind::kB);
                        break;
                    case SDLK_SPACE:
                        joypad.unpress(ButtonKind::kStart);
                        break;
                }
            }
            if (e.type == SDL_QUIT) quit = true;
        }
        gb.run();
    }

}