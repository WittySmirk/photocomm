#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "shared.h"

void renderText(SDL_Renderer* renderer, TTF_Font* font, char *text, int x, int y, bool center, SDL_Color color);
// bool transmit(char *message, char *byteString);
void displaybyte(unsigned char b, char* byteString);

int transmitter();

#endif
