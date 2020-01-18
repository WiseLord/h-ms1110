#include "canvas.h"

#include "../audio/audio.h"
#include "../input.h"
#include "../display/glcd.h"
#include "../tr/labels.h"

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

void canvasShowSpectrum(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);

    glcdWriteString("Spectrum");
}

void canvasShowTime(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);

    glcdWriteString("Time");
}

void canvasShowStandby(bool clear)
{
    (void)clear;

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

    AnalogBtn aBtn = inputGetCtx()->aBtn;

    for (AnalogBtn i = 0; i < ABTN_END; i++) {
        glcdDrawCircle(16 + 20 * i, 40, 6, i == aBtn ? LCD_COLOR_WHITE : LCD_COLOR_GRAY);
    }
}

void canvasShowTune(bool clear)
{
    (void)clear;
    canvasClear();

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    const int16_t value = aProc->par.tune[aProc->tune].value;
//    Icon icon = (ICON_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
//        icon = ICON_VOLUME + aProc->tune;
    }

    const AudioGrid *grid = aProc->par.tune[aProc->tune].grid;
//    const int8_t min = grid ? grid->min : 0;
//    const int8_t max = grid ? grid->max : 0;
    const uint8_t mStep = grid ? grid->mStep : 0;


    glcdSetFont(&fontterminus32);
    glcdSetXY(0, 0);
    glcdWriteString(label);

    glcdSetFont(&fontterminus32);
    glcdSetXY(256, 0);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdWriteString(utilMkStr("%3d", value * mStep / 8));
}
