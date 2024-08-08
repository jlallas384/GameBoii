#include "sdl_lcd.h"

//extern std::chrono::steady_clock::time_point begin;

SDLLCD::SDLLCD(SDL_Window* window) : 
    renderer(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)), 
    texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, width, height)) {
    SDL_RenderSetLogicalSize(renderer, width, height);
}

void SDLLCD::setPixel(uint8_t i, uint8_t j, uint16_t color) {
    buffer[i][j] = color;
}

void SDLLCD::refresh() {
    //static uint8_t x = 0;
    SDL_UpdateTexture(texture, nullptr, buffer, width * sizeof(uint16_t));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    //x++;
    //if (x == 60) {
    //    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    //    x = 0;
    //    begin = end;
    //}
}