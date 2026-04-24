#ifndef SHARED_H
#define SHARED_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "stdbool.h"

#define SCREEN_W 1280
#define SCREEN_H 720

static const SDL_Color CRUST = {.r = 17, .g = 17, .b = 27, .a = 255}; //rgb(17, 17, 27)
static const SDL_Color ROSEWATER = {.r = 245, .g = 224, .b = 220, .a = 255}; // rgb(245, 224, 220)
static const SDL_Color BACKGROUND = {.r = 30, .g = 30, .b = 46, .a = 255}; // rgb(30, 30, 46)

void renderText(SDL_Renderer* renderer, TTF_Font* font, char *text, int x, int y, bool center, SDL_Color color);

#endif
