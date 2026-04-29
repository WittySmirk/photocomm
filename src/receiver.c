#include "receiver.h"

/*
int receiver() {
    const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!codec) {
        fprintf(stderr, "Codec not found\n");
        return -1;
    }

    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    if(!codecCtx) {
        fprintf(stderr, "Could not allocate codec context\n");
        return -1;
    }

    if(avcodec_open2(codecCtx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    AVFrame*  frame  = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();

    if(!frame || !packet) {
        fprintf(stderr, "Could not allocate frame/packet\n");
        return -1;
    }

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window*   window   = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture*  texture  = NULL;

    uint32_t size;
    uint8_t* buf    = NULL;
    uint32_t bufCap = 0;

    while(fread(&size, sizeof(uint32_t), 1, stdin) == 1) {
        if(size == 0 || size > 10 * 1024 * 1024) {
            fprintf(stderr, "Invalid packet size: %u\n", size);
            break;
        }

        if(size > bufCap) {
            uint8_t* newBuf = realloc(buf, size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!newBuf) {
                fprintf(stderr, "Out of memory\n");
                break;
            }
            buf    = newBuf;
            bufCap = size;
        }

        memset(buf + size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

        if(fread(buf, 1, size, stdin) != size) {
            fprintf(stderr, "Incomplete read\n");
            break;
        }

        av_new_packet(packet, size);
        memcpy(packet->data, buf, size);

        if(avcodec_send_packet(codecCtx, packet) < 0) {
            fprintf(stderr, "Failed to send packet\n");
            av_packet_unref(packet);
            continue;
        }
        av_packet_unref(packet);

        int ret;
        while((ret = avcodec_receive_frame(codecCtx, frame)) >= 0) {
            int w = frame->width, h = frame->height;

            if(!window) {
                window = SDL_CreateWindow("PhotoComm",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    w, h, 0);
                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                texture  = SDL_CreateTexture(renderer,
                    SDL_PIXELFORMAT_IYUV,
                    SDL_TEXTUREACCESS_STREAMING, w, h);

                if (!window || !renderer || !texture) {
                    fprintf(stderr, "SDL setup failed: %s\n", SDL_GetError());
                    goto done;
                }
            }

            SDL_UpdateYUVTexture(texture, NULL,
                frame->data[0], frame->linesize[0],
                frame->data[1], frame->linesize[1],
                frame->data[2], frame->linesize[2]);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_Event e;
            while (SDL_PollEvent(&e))
                if (e.type == SDL_QUIT) goto done;

            av_frame_unref(frame);
}

        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            fprintf(stderr, "Error receiving frame: %s\n", av_err2str(ret));
            break;
        }
    }

done:
    free(buf);
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codecCtx);
    if (texture)  SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
*/

void *rx_thread(void *arg) {
    int idx = 0;
    while (running) {
        int b = rx_byte();
        if (b >= 0 && idx < RX_BUF - 1) {
            rx_result[idx++] = (char)b;
            rx_result[idx] = '\0';
            rx_done = 1;
        }
    }
    return NULL;
}

int rx_byte(void) {
    int timeout_us = 250000;

    /* wait for start bit — line leaves idle state */
    while (lgGpioRead(gh, RX_GPIO) == RX_IDLE_LEVEL) {
        delay_us(1);
        if (--timeout_us <= 0) return -1;
    }

    /* sample middle of start bit to confirm genuine */
    delay_us(HALF_BIT);
    if (lgGpioRead(gh, RX_GPIO) == RX_IDLE_LEVEL) return -1;

    unsigned char b = 0;
    for (int i = 0; i < 8; i++) {
        delay_us(BIT_US);
        int bit = lgGpioRead(gh, RX_GPIO);
        if (RX_IDLE_LEVEL == 0) bit = !bit;
        if (bit) b |= (1 << i);
    }

    delay_us(BIT_US); /* consume stop bit */
    return (int)b;
}

void print_byte(int idx, unsigned char b) {
    printf("byte %2d: ", idx);
    for (int i = 7; i >= 0; i--)
        printf("%d", (b >> i) & 1);
    printf("0x%02X  '%c'\n", b, (b >= 32 && b < 127) ? b : '.');
    fflush(stdout);
}

int receiver() {
    gh = lgGpiochipOpen(GPIOCHIP);
    if (gh < 0) {
        fprintf(stderr, "Failed to open gpiochip%d — run with sudo\n", GPIOCHIP);
        return 1;
    }
    
    lgGpioClaimInput(gh, LG_SET_PULL_UP, RX_GPIO);

    pthread_t tid;
    pthread_create(&tid, NULL, rx_thread, NULL);

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf failed: %s\n", TTF_GetError());
    }
    
    SDL_Window* window = SDL_CreateWindow("photocomm - receiver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event e;

    TTF_Font* font = TTF_OpenFont("res/FiraSans-SemiBold.ttf", 100);

    //TODO: extend logic to work better
    while(running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g, BACKGROUND.b, BACKGROUND.a); //rgb(30, 30, 46)

        SDL_RenderClear(renderer);
        
        if (strlen(rx_result) == 0) {
            TTF_SetFontSize(font, 100);
            renderText(renderer, font, "Send a message", 0, 0, true, CRUST);
        } else {
            TTF_SetFontSize(font, 30);
            renderText(renderer, font, rx_result, 0, 0, true, ROSEWATER);
        }

        SDL_RenderPresent(renderer);
    }
    
    pthread_join(tid, NULL);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
