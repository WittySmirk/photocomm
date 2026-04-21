#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include "SDL.h"
#include "SDL_ttf.h"
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define SCREEN_W 1280
#define SCREEN_H 720

bool transmit(char *message);
void displaybyte(unsigned char b);

int transmitter();

#endif