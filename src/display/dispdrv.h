#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../pins.h"
#include "../utils.h"
#include "fonts.h"

#ifdef _DISP_SPI
#include "../spi.h"
#define SPI_DISPLAY             SPI2
#define DISP_WAIT_BUSY()        spiWaitBusy(SPI_DISPLAY)
#else
#define DISP_WAIT_BUSY()        (void)0
#endif

#define LCD_ROTATE_0        0x00
#define LCD_ROTATE_180      0x02

#define LCD_BR_MIN          1
#define LCD_BR_MAX          32

typedef uint8_t color_t;

typedef struct {
    void (*init)(void);
    void (*sleep)(void);
    void (*wakeup)(void);
    void (*setWindow)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*rotate)(uint8_t rotate);
    void (*shift)(int16_t value);

    void *fb;
    void (*update)(void);
    void (*sendColor)(color_t data);

    int16_t width;
    int16_t height;
} DispDriver;

extern const DispDriver dispdrv;

void dispdrvReset(void);
void dispdrvInit(void);

void dispdrvSendData8(uint8_t data);

void dispdrvSelectReg8(uint8_t reg);

void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color);
void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
