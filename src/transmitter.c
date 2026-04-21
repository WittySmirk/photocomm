#include "transmitter.h"

/*
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

void displaybyte(unsigned char b) {
    // TODO: make this display on SDL bit-by-bit
    for (int i = 7; i >= 0; i--) {
        printf("%d", (b >> i) & 1);
    }
    printf("\n");
}

int transmitter() {
    int fd = open("/dev/cu.usbmodem21301", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tcsetattr(fd, TCSANOW, &tty);

    const char * message = "Hello World";

    size_t len = strlen(message);
    for (int i = 0; i < len; i++) {
        displaybyte((unsigned char)message[i]);
        write(fd, &message[i], 1);
        usleep(50000);
    }

    close(fd);
    return 0;
}