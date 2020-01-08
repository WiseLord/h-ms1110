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

    uint16_t *adcData = inputAnalogGetData();

    glcdSetXY(0, h / 16 * 2);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_BTN]));

    glcdSetXY(0, h / 16 * 7);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_POT_A]));

    glcdSetXY(0, h / 16 * 12);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_POT_B]));


    int16_t potA = 200 - 200 * adcData[AIN_POT_A] / 4096;
    int16_t potB = 200 - 200 * adcData[AIN_POT_B] / 4096;

    glcdDrawRect(40, h / 16 * 2, potA, 10, LCD_COLOR_WHITE);
    glcdDrawRect(40 + potA, h / 16 * 2, 200 - potA, 10, LCD_COLOR_NAVI);

    glcdDrawRect(40, h / 16 * 7, potB, 10, LCD_COLOR_WHITE);
    glcdDrawRect(40 + potB, h / 16 * 7, 200 - potB, 10, LCD_COLOR_NAVI);

}
