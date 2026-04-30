#ifndef RECIEVER_H
#define RECIEVER_H
#include "SDL.h"
#include "SDL_ttf.h"
#include "stdbool.h"
#include "shared.h"
#include "lgpio.h" //TODO: make cmake have command to make either transmitter or receiver
#include "stdio.h"
#include "pthread.h"

#define GPIOCHIP 0 
#define RX_GPIO 24
#define RX_IDLE_LEVEL 0 

#define HALF_BIT (BIT_US / 2.0)
#define MSG_COUNT 2

static int gh = -1;
static volatile int running = 1;

#define RX_BUF 256
static char rx_result[RX_BUF];
static volatile int rx_done = 0;

static inline void delay_us(double us) { lguSleep(us / 1e6); }

static pthread_t rx_tid;
static volatile int rx_running = 0;

void *rx_thread(void *arg);
int rx_byte(void);
void print_byte(int idx, unsigned char b);

int receiver();

#endif
