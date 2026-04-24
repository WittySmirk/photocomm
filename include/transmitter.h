#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define SCREEN_W 1280
#define SCREEN_H 720

static const SDL_Color CRUST = {.r = 17, .g = 17, .b = 27, .a = 255}; //rgb(17, 17, 27)
static const SDL_Color ROSEWATER = {.r = 245, .g = 224, .b = 220, .a = 255}; // rgb(245, 224, 220)

void renderText(SDL_Renderer* renderer, TTF_Font* font, char *text, int x, int y, bool center, SDL_Color color);
// bool transmit(char *message, char *byteString);
void displaybyte(unsigned char b, char* byteString);

int transmitter();

#endif
