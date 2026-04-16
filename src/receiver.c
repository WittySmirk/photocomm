#include "receiver.h"

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
                window = SDL_CreateWindow("Stream Viewer",
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