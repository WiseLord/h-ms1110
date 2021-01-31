#include "demo.h"

#include <stdlib.h>

#include "display/glcd.h"
#include "fft.h"

typedef struct {
    uint8_t frameNum;
    uint8_t size;
    int16_t angle;
} Star;

#define STAR_NUM    64
#define FRAMES      50

static Star stars[STAR_NUM];

static int16_t getDistance(uint8_t frameNum)
{
    return ((FRAMES - frameNum + 1) * (FRAMES - frameNum) / 2 + 1024) / 8 - 128;
}

void drawStar(Star *star, color_t color, int16_t offset)
{
    int16_t distance = getDistance(star->frameNum);

    int16_t x = fft_cos(star->angle) * distance / 32768;
    int16_t y = fft_sin(star->angle) * distance / 32768;

    if (color == COLOR_WHITE) {
        color = COLOR_GRAY16(star->size);
    }
    glcdDrawPixel(128 + x, offset + y, color);
}

void demoDraw(int16_t offset)
{
    for (int i = 0; i < STAR_NUM; i++) {
        if (stars[i].frameNum == 0) {
            stars[i].frameNum = (rand() % FRAMES) * (rand() % FRAMES) / FRAMES;
            stars[i].size = 0;
            stars[i].angle = rand() % 1024;
        } else {
            drawStar(&stars[i], COLOR_BLACK, offset);
            stars[i].frameNum--;
            if (stars[i].size < 15) {
                stars[i].size++;
            }
        }
        drawStar(&stars[i], COLOR_WHITE, offset);
    }
}
