#include "sdl_lcd.h"

SDLLCD::SDLLCD(SDL_Window* window) : 
    renderer(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)), 
    texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, width, height)) {
    SDL_RenderSetLogicalSize(renderer, width, height);
}

void SDLLCD::setPixel(uint8_t i, uint8_t j, uint16_t color) {
    buffer[i][j] = color;
}

void SDLLCD::refresh() {
    SDL_UpdateTexture(texture, nullptr, buffer, width * sizeof(uint16_t));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}