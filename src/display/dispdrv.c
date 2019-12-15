#include "dispdrv.h"

#include <stdbool.h>

void dispdrvReset(void)
{
    spiInit(SPI_DISPLAY, false);

    SET(DISP_CS);
    SET(DISP_RS);

    CLR(DISP_RST);
    utilmDelay(50);
    SET(DISP_RST);

    utilmDelay(50);
}

void dispdrvInit(void)
{
    dispdrvReset();

    dispdrv.init();
}

void dispdrvSendData8(uint8_t data)
{
    spiSendByte(SPI_DISPLAY, data);
}

void dispdrvSelectReg8(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    spiSendByte(SPI_DISPLAY, reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color)
{
    dispdrv.setWindow(x, y, 1, 1);
    dispdrv.sendColor(color);
}

void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    dispdrv.setWindow(x, y, w, h);

    for (int32_t i = 0; i < w * h; i++) {
        dispdrv.sendColor(color);
    }
}

void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h)
{
    dispdrv.setWindow(x, y, w, h);

    for (int16_t i = 0; i < w; i++) {
        for (int16_t j = 0; j < h; j++) {
            uint8_t data = img->data[img->width * ((j + yOft) >> 3) + i + xOft];
            if (j < h) {
                dispdrv.sendColor(data & (1 << ((j + yOft) & 0x7)) ? color : bgColor);
            }
        }
    }
}
