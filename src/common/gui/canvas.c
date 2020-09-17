#include "canvas.h"

#include <stdio.h>
#include <string.h>

#include "amp.h"
#include "input/matrix.h"
#include "rtc.h"
#include "screen/spectrumview.h"
#include "screen/setupview.h"
#include "screen/timeview.h"
#include "swtimers.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit(GLCD_LANDSCAPE);

    canvas.glcd = glcdGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet();

    // Default font parameters
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);

    GlcdRect *rect = &canvas.glcd->rect;

    glcdDrawRect(rect->x, rect->y, rect->w, rect->h, canvas.pal->bg);
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;
    const Palette *pal = canvas.pal;

    glcdDrawRect(0, 0, rect.w, rect.h, pal->bg);
    glcdShift(0);

    glcdSetFontColor(pal->fg);
    glcdSetFontBgColor(pal->bg);
}

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks)
{
    GlcdRect rect = glcdGet()->rect;

    switch (mode) {
    case SP_MODE_LEFT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        break;
    case SP_MODE_RIGHT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_LEFT_MIRROR:
        spViewDraw(clear, true, true, peaks, SP_CHAN_LEFT, &rect);
        break;
    case SP_MODE_RIGHT_MIRROR:
        spViewDraw(clear, true, true, peaks, SP_CHAN_RIGHT, &rect);
        break;

    case SP_MODE_STEREO:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_MIRROR:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, true, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_ANTIMIRROR:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, true, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_MIXED:
        spViewDraw(clear, true, false, peaks, SP_CHAN_BOTH, &rect);
        break;
    default:
        break;
    }
}

void canvasShowTime(bool clear, bool active)
{
    RTC_type rtc;
    rtcGetTime(&rtc);

    timeViewDraw(clear, active, rtc.hour, rtc.min, rtc.sec);
}

void canvasShowDate(bool clear, bool active)
{
    RTC_type rtc;
    rtcGetTime(&rtc);

    dateViewDraw(clear, active, rtc.date, rtc.month, rtc.year, rtc.wday);
}

void canvasShowWday(bool clear, bool active)
{
    RTC_type rtc;
    rtcGetTime(&rtc);

    wdayViewDraw(clear, active, rtc.wday);
}

void canvasShowInput(bool clear, int8_t input, Label label)
{
    (void)clear;

    const Palette *pal = canvas.pal;
    GlcdRect rect = canvas.glcd->rect;

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    char buf[32];
    snprintf(buf, sizeof(buf), "%d : %s", input, labelsGet(label));

    glcdSetXY(rect.w / 2, 28);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(buf);
}

void canvasShowTune(bool clear, TuneView *tune)
{
    tuneViewDraw(clear, tune);
}

void canvasShowSetup(bool clear)
{
    setupViewDraw(clear);
}

void canvasShowRadio(bool clear, RadioView *radio)
{
    radioViewDraw(clear, radio);
}

void canvasDebugFPS(void)
{
    const Palette *pal = canvas.pal;

    glcdSetFont(&fontterminus12);
    glcdSetFontColor(pal->inactive);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    static int32_t oldCnt = 0;
    static int32_t oldFps = 0;
    static int32_t frames = 0;

    frames++;

    int32_t cnt = swTimGet(SW_TIM_SYSTEM);
    if (cnt - oldCnt > 500) {
        int32_t fps = frames * 1000 / (cnt - oldCnt);
        frames = 0;
        oldFps = fps;
        oldCnt = cnt;
    } else {
    }

    char buf[16];

    glcdSetXY(canvas.glcd->rect.w, 52);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    snprintf(buf, sizeof(buf), "%d", (int)oldFps);
    glcdWriteString(buf);
}
