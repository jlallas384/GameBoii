#include "application.h"
#include <chrono>
#include <SDL2/SDL.h>
#include "sdl_lcd.h"

Application::Application() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("gbmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160 * 2, 144 * 2, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    auto lcd = std::make_unique<SDLLCD>(window);
    gameBoy = std::make_unique<GameBoy>(std::move(lcd));
}

Application::~Application() {
    SDL_DestroyWindow(window);
}

void Application::run() {
    using namespace std::chrono_literals;
    std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
    while (!quit) {
        gameBoy->run();
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if (now - last >= 16670us) {
            for (int i = 0; i < 17556; i++) {
                gameBoy->run();
                if (i % 10 == 0) {
                    pollEvents();
                }
            }
            last = now;
        }

    }
}

void Application::pollEvents() {
    Joypad& joypad = gameBoy->getJoypad();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_DROPFILE:
                gameBoy->loadCartridge(e.drop.file);
                SDL_free(e.drop.file);
                break;
            case SDL_KEYDOWN:
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
                break;
            case SDL_KEYUP:
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
                break;
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
        }
    }
}
