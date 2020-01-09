#include "canvas.h"

#include "../input.h"
#include "../display/glcd.h"

static Canvas canvas;

#define R_H             4700
#define R_POT           100000

#define ZONE_CNT        31

#define ADC_MAX         (4095 * R_POT / (R_POT + R_H))

#define POT_ZONE_LEN    (ADC_MAX / ZONE_CNT)

#define ABS(x)          ((x) >= 0 ? (x) : (-x))

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

    int16_t *adcData = inputAnalogGetData();

    int16_t potA = ADC_MAX - adcData[AIN_POT_A];
    int16_t potB = ADC_MAX - adcData[AIN_POT_B];
    int16_t btn = adcData[AIN_BTN];

    glcdSetXY(0, 48);
    glcdWriteString(utilMkStr("%4d", btn));

    const int16_t sw = 256;

    int16_t lenA = sw * (ADC_MAX - adcData[AIN_POT_A]) / ADC_MAX;
    int16_t lenB = sw * (ADC_MAX - adcData[AIN_POT_B]) / ADC_MAX;

    glcdDrawRect(0, 0, lenA, 10, LCD_COLOR_WHITE);
    glcdDrawRect(0 + lenA, 0, sw - lenA, 10, LCD_COLOR_NAVI);

    glcdDrawRect(0, 16, lenB, 10, LCD_COLOR_WHITE);
    glcdDrawRect(0 + lenB, 16, sw - lenB, 10, LCD_COLOR_NAVI);

    glcdDrawRect(0, 32, sw, 10, LCD_COLOR_NAVI);

    for (int16_t i = 1; i < ZONE_CNT; i++) {
        glcdDrawRect(0 + i * sw / ZONE_CNT, 32, 1, 10, LCD_COLOR_BLACK);
    }

    static int16_t prevPotA = POT_ZONE_LEN / 2;
    static int16_t prevPotB = POT_ZONE_LEN / 2;

    int16_t diffPotA = potA - prevPotA;
    int16_t diffPotB = potB - prevPotB;

    if (ABS(diffPotA) > POT_ZONE_LEN) {
        prevPotA = (potA * ZONE_CNT / ADC_MAX) * POT_ZONE_LEN + POT_ZONE_LEN / 2;
    }
    if (ABS(diffPotB) > POT_ZONE_LEN) {
        prevPotB = (potB * ZONE_CNT / ADC_MAX) * POT_ZONE_LEN + POT_ZONE_LEN / 2;
    }

    int16_t zoneA = prevPotA * ZONE_CNT / ADC_MAX;
    int16_t zoneB = prevPotB * ZONE_CNT / ADC_MAX;

    glcdSetXY(40, 48);
    glcdWriteString(utilMkStr("%2d %4d %2d", prevPotA, potA, zoneA - (ZONE_CNT / 2)));
    glcdSetXY(40 + sw / 2, 48);
    glcdWriteString(utilMkStr("%2d %4d %2d", prevPotB, potB, zoneB - (ZONE_CNT / 2)));
}
