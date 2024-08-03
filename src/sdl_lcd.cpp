#include "sdl_lcd.h"
#include <iostream>
#include "timer.h"

extern std::chrono::steady_clock::time_point begin;

SDLLCD::SDLLCD(SDL_Window* window) : window(window), surface(SDL_GetWindowSurface(window)) {

}

void SDLLCD::setPixel(uint8_t i, uint8_t j, uint8_t color) {
    SDL_Rect px = { j * 3, i * 3, 3, 3 };
    static Uint32 colors[] = {
        SDL_MapRGB(surface->format, 255, 255, 255),
        SDL_MapRGB(surface->format, 211, 211, 211),
        SDL_MapRGB(surface->format, 169, 169, 169),
        SDL_MapRGB(surface->format, 0, 0, 0)
    };
    SDL_FillRect(surface, &px, colors[color]);

}

void SDLLCD::refresh() {
    static uint8_t x = 0;
    SDL_UpdateWindowSurface(window);
    x++;
    if (x == 60) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
        x = 0;
        begin = end;
    }
}