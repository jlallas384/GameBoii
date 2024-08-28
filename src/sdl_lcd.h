#pragma once

#include "ppu/lcd.h"
#include <SDL2/SDL.h>

class SDLLCD : public LCD {
public:
    SDLLCD(SDL_Window* window);
    void refresh() override;
    void setPixel(uint8_t i, uint8_t j, Color color) override;
private:
    Color buffer[height][width] = {};
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};