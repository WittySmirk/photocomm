#include "transmitter.h"

/*
TODO: transmit video
int transmitter() {
    AVFormatContext* ctx = NULL;

    if (avformat_open_input(&ctx, "./test.mp4", NULL, NULL) < 0) {
        printf("Failed to open file\n");
        return -1;
    }
    if (avformat_find_stream_info(ctx, NULL) < 0) {
        printf("Could not find stream info\n");
        return -1;
    }

    int videoStream = -1;

    for (unsigned int i = 0; i < ctx->nb_streams; i++) {
        if(ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
            videoStream = i;
        }
    }

    if (videoStream == -1) {
        printf("Could not find video stream\n");
        return -1;
    }

    AVCodecParameters* par = ctx->streams[videoStream]->codecpar;
    fprintf(stderr, "width=%d height=%d\n", par->width, par->height);

    // Annex B filter for SDL
    const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
    AVBSFContext* bsfCtx = NULL;
    av_bsf_alloc(bsf, &bsfCtx);
    avcodec_parameters_copy(bsfCtx->par_in, par);
    av_bsf_init(bsfCtx);

    AVPacket* packet = av_packet_alloc();

    while(av_read_frame(ctx, packet) >= 0) {
        if(packet->stream_index == videoStream) {
            av_bsf_send_packet(bsfCtx, packet);

            while(av_bsf_receive_packet(bsfCtx, packet) >= 0) {
                uint32_t size = packet->size;
                fwrite(&size, sizeof(uint32_t), 1, stdout);
                fwrite(packet->data, 1, packet->size, stdout);
                av_packet_unref(packet);
            }
        } else {
            av_packet_unref(packet);
        }
    }

    av_packet_free(&packet);
    av_bsf_free(&bsfCtx);
    avformat_close_input(&ctx);

    return 0;
}
*/

void renderText(SDL_Renderer* renderer, TTF_Font* font, char *text, int x, int y, bool center, SDL_Color color) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        // Query dimensions from the texture, not the freed surface
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect dst = {};
        if (center) {
            dst = (SDL_Rect){.x = (SCREEN_W / 2) - (w / 2), .y = (SCREEN_H / 2) - (h / 2), .w = w, .h = h};
        } else {
            dst = (SDL_Rect){.x = x, .y = y, .w = w, .h = h};
        }

        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
}

void displaybyte(unsigned char b, char* byteString) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (b >> i) & 1);
        char bit = '0' + ((b >> i) & 1);
        if (strlen(byteString) != 0) {
            strncat(byteString, " ", 1);
        }
        strncat(byteString, &bit, 1);

    }
    printf("\n");
}


int transmitter() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf failed: %s\n", TTF_GetError());
    }

    SDL_Window* window = SDL_CreateWindow("photocomm - transmitter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event e;

    TTF_Font* font = TTF_OpenFont("res/FiraSans-SemiBold.ttf", 100);

    char message[256] = "";
    char byteString[256] = "";

    bool transmitting = false;
    int index = 0;
    int fd = -1;
    struct termios tty;

    size_t len = 0;

    while(running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                    break;
                    case SDLK_BACKSPACE:
                        if (strlen(message) > 0) {
                            message[strlen(message) - 1] = '\0';
                        }
                    break;
                    case SDLK_RETURN:
                        transmitting = true;
                    break;
                    default:
                        char key = (char)e.key.keysym.sym;
                        strncat(message, &key, 1);
                    break;
                }
            }
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        if (transmitting) {
            if (fd == -1) {
                byteString[0] = '\0';
                fd = open("/dev/cu.usbmodem21301", O_RDWR | O_NOCTTY);
                if (fd < 0) {
                    fprintf(stderr, "open error\n");
                    return false;
                }

                tcgetattr(fd, &tty);
                cfsetospeed(&tty, B9600);
                cfsetispeed(&tty, B9600);
                tcsetattr(fd, TCSANOW, &tty);

                len = strlen(message);
            }

            if (index < len) {
                displaybyte((unsigned char)message[index], byteString);
                write(fd, &message[index], 1);
                usleep(50000);
                index++;
            } else {
                close(fd);
                fd = -1;
                index = 0;
                len = 0;
                transmitting = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 46, 255); //rgb(30, 30, 46)

		SDL_RenderClear(renderer);
        
        if (!transmitting) {
            TTF_SetFontSize(font, 100);
            if (strlen(message) == 0) {
                renderText(renderer, font, "Enter text", 0, 0, true, CRUST);
            } else {
                renderText(renderer, font, message, 0, 0, true, ROSEWATER);
            }
        }
        
        if (transmitting) {
            TTF_SetFontSize(font, 30);
            renderText(renderer, font, byteString, 0, 0, true, ROSEWATER);
        }

        SDL_RenderPresent(renderer);
    }

    if (fd != -1) close(fd);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}