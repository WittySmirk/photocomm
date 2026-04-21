#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

void displaybyte(unsigned char b);

int transmitter();

#endif