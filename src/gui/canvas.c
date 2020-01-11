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

    glcdSetFont(&fontterminus12);

    InputCtx *inCtx = inputGetCtx();

    const int16_t sw = 210;
    const int16_t zoneCnt = inCtx->zoneCnt;

    for (uint8_t i = AIN_POT_A; i < AIN_POT_END; i++) {
        int8_t pot = inputGetPot(i);
        int16_t len = (sw * pot + sw / 2) / zoneCnt;

        int16_t xb = 0;
        int16_t yb = 16 * i;

        glcdDrawRect(xb, yb, len, 12, LCD_COLOR_WHITE);
        glcdDrawRect(xb + len, yb, sw - len, 12, LCD_COLOR_GRAY);
        glcdSetXY(xb + sw + 5, yb);
        glcdWriteString(utilMkStr("%2d %4d", pot, inCtx->potData[i]));
        for (int16_t j = 1; j < zoneCnt; j++) {
            glcdDrawRect(xb + j * sw / zoneCnt, yb, 1, 12, RGB_CONV(0xA0A0A0));
        }
    }

    AnalogBtn aBtn = inputGetAnalogBtn();

    for (AnalogBtn i = 0; i < ABTN_END; i++) {
        glcdDrawCircle(16 + 20 * i, 40, 6, i == aBtn ? LCD_COLOR_WHITE : LCD_COLOR_GRAY);
    }
}
