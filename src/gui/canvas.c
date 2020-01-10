#include "canvas.h"

#include "../input.h"
#include "../display/glcd.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit(&canvas.glcd);

    canvasClear();
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, LCD_COLOR_BLACK);
    glcdShift(0);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
}

void canvasShowStandby(bool clear)
{
    (void)clear;

    // Graphics
    int16_t w = canvas.glcd->drv->width;
    int16_t h = canvas.glcd->drv->height;

    glcdDrawRect(0, 0, w, h, LCD_COLOR_BLACK);

    glcdSetFont(&fontterminus16);

    InputCtx *inCtx = inputGetCtx();

    const int16_t sw = 217;
    const int16_t zoneCnt = inCtx->zoneCnt;

    int16_t len;
    int8_t potA = inputGetPot(AIN_POT_A);
    int8_t potB = inputGetPot(AIN_POT_B);

    len = (sw * potA + sw / 2) / zoneCnt;
    glcdDrawRect(0, 0, len, 10, LCD_COLOR_WHITE);
    glcdDrawRect(0 + len, 0, sw - len, 10, LCD_COLOR_GRAY);

    len = (sw * potB + sw / 2) / zoneCnt;
    glcdDrawRect(0, 16, len, 10, LCD_COLOR_WHITE);
    glcdDrawRect(0 + len, 16, sw - len, 10, LCD_COLOR_GRAY);


    glcdDrawRect(0, 32, sw, 10, LCD_COLOR_GRAY);
    for (int16_t i = 1; i < zoneCnt; i += 2) {
        glcdDrawRect(0 + i * sw / zoneCnt, 32, sw / zoneCnt, 10, RGB_CONV(0xA0A0A0));
    }

    glcdSetXY(0, 48);
    glcdWriteString(utilMkStr("%4d %4d %2d", inCtx->potData[AIN_POT_A], POT_MAX - inCtx->adcData[AIN_POT_A], potA));
    glcdSetXY(sw / 2, 48);
    glcdWriteString(utilMkStr("%4d %4d %2d", inCtx->potData[AIN_POT_B], POT_MAX - inCtx->adcData[AIN_POT_B], potB));
}
