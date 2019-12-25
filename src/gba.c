#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *) 0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
    while (*SCANLINECOUNTER < 160);
    while (*SCANLINECOUNTER > 160);
    vBlankCounter++;
}

static int __qran_seed = 42;
static int qran(void) {
    __qran_seed = 1664525 * __qran_seed + 1013904223;
    return (__qran_seed >> 16) & 0x7FFF;
}

void seed_qran(int seed) {
    __qran_seed = seed;
}

int randint(int min, int max) {
    return (qran() * (max - min) >> 15) + min;
}

__attribute__((always_inline))
inline void setPixel(int x, int y, u16 color) {
    *(videoBuffer + PIXEL(x, y)) = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color) {
    int yt = y + height;
    for (int r = y; r < yt; r++) {
        DMA[DMA_CHANNEL_3].src = &color;
        DMA[DMA_CHANNEL_3].dst = &videoBuffer[PIXEL(x, r)];
        DMA[DMA_CHANNEL_3].cnt = DMA_ON | DMA_SOURCE_FIXED | width;
    }
}

void drawFullScreenImageDMA(const u16 *image) {
    DMA[DMA_CHANNEL_3].src = image;
    DMA[DMA_CHANNEL_3].dst = videoBuffer;
    DMA[DMA_CHANNEL_3].cnt = DMA_ON | DMA_32 | (PIXELS / 2);
}

void drawImageDMA(int x, int y, int width, int height, const u16 *image) {
    for (int r = 0; r < height; r++) {
        DMA[DMA_CHANNEL_3].src = &image[width * r];
        DMA[DMA_CHANNEL_3].dst = &videoBuffer[PIXEL(x, r + y)];
        DMA[DMA_CHANNEL_3].cnt = DMA_ON | width;
    }
}

void fillScreenDMA(volatile u16 color) {
    volatile u32 color32 = color << 16 | color;
    DMA[DMA_CHANNEL_3].src = &color32;
    DMA[DMA_CHANNEL_3].dst = videoBuffer;
    DMA[DMA_CHANNEL_3].cnt = DMA_ON | DMA_32 | DMA_SOURCE_FIXED | (PIXELS / 2);
}

void drawChar(int x, int y, char ch, u16 color) {
    unsigned char const *src = &fontdata_6x8[ch * 48];
    volatile u16 *dst = &videoBuffer[x + y * WIDTH];
    for (int j = 0; j < 8; j++, dst += WIDTH - 6) {
        for (int i = 0; i < 6; i++, src++, dst++) {
            if (*src) {
                *dst = color;
            }
        }
    }
}

void drawString(int x, int y, char *str, u16 color) {
    while (*str) {
        drawChar(x, y, *str++, color);
        x += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}

void overlayChar(int x, int y, char ch, u16 fg, u16 bg) {
    unsigned char const *src = &fontdata_6x8[ch * 48];
    volatile u16 *dst = &videoBuffer[x + y * WIDTH];
    for (int j = 0; j < 8; j++, dst += WIDTH - 6) {
        for (int i = 0; i < 6; i++, src++, dst++) {
            if (*src) {
                *dst = fg;
            } else {
                *dst = bg;
            }
        }
    }
}

void overlayString(int x, int y, char *str, u16 fg, u16 bg) {
    int len = 0;
    char *s = str;
    while (*s++) {
        len++;
    }
    drawRectDMA(x, y, len * 6, 8, bg);
    while (*str) {
        overlayChar(x, y, *str++, fg, bg);
        x += 6;
    }
}

