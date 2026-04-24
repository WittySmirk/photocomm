#ifndef RECIEVER_H
#define RECIEVER_H
#include "SDL.h"
#include "SDL_ttf.h"
#include "stdbool.h"
#include "shared.h"
#include "lgpio.h" //TODO: make cmake have command to make either transmitter or receiver
#include "stdio.h"

#define GPIOCHIP 4
#define RX_GPIO 24
#define RX_IDLE_LEVEL 1 // idle HIGH (dark)

#define BAUD 1200
#define BIT_US (1000000.0 / BAUD)
#define HALF_BIT (BIT_US / 2.0)
#define MSG_COUNT 2

static int gh = -1;
static volatile int running = 1;

#define RX_BUF 256
static char rx_result[RX_BUF];
static volatile int rx_done = 0;

int rx_byte(void);
void print_byte(int idx, unsigned char b);

int receiver();

#endif
