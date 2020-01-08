#include "canvas.h"

#include "../input.h"
#include "../display/glcd.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit(&canvas.glcd);

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

    int16_t tw = w / 16;
    int16_t th = h / 4;

    glcdDrawRect(w / 2 + tw * 1, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_NERO);
    glcdDrawRect(w / 2 + tw * 3, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_GRAY);
    glcdDrawRect(w / 2 + tw * 5, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_ECLIPSE);

    glcdSetFont(&fontterminus16);

    uint16_t *adcData = inputAnalogGetData();

    glcdSetXY(0, h / 16 * 2);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_BTN]));

    glcdSetXY(0, h / 16 * 7);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_POT_A]));

    glcdSetXY(0, h / 16 * 12);
    glcdWriteString(utilMkStr("%4u", adcData[AIN_POT_B]));
}
