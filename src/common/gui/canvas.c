#include "canvas.h"

#include <string.h>

#include "rtc.h"
#include "swtimers.h"
#include "widget/spview.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet();

    // Default font parameters
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);
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
    case SP_MODE_LEFT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        break;
    case SP_MODE_RIGHT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    default:
        break;
    }
}

void canvasShowTime(bool clear, bool active)
{
    const Palette *pal = canvas.pal;

    const tFont *font = &fontterminusdig40;

    glcdSetFont(font);
    glcdSetFontColor(active ? pal->active : pal->inactive);

    GlcdRect rect = glcdGet()->rect;
    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetXY(rect.w / 2, 12);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(utilMkStr("%02d:%02d:%02d", rtc.hour, rtc.min, rtc.sec));

}

void canvasShowDate(bool clear, bool active)
{
    const Palette *pal = canvas.pal;

    const tFont *font = &fontterminus32;

    glcdSetFont(font);
    glcdSetFontColor(active ? pal->active : pal->inactive);

    GlcdRect rect = glcdGet()->rect;
    RTC_type rtc;
    rtcGetTime(&rtc);

    const char *monthLabel = labelsGet((Label)(LABEL_JANUARY + rtc.month - 1));

    glcdSetXY(rect.w / 2, 0);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(utilMkStr("%02d %s 20%02d", rtc.date, monthLabel, rtc.year));

    const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + rtc.wday));

    glcdSetXY(rect.w / 2, 32);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(wdayLabel);
}

void canvasShowInput(bool clear, Label label)
{
    const Palette *pal = canvas.pal;
    GlcdRect rect = canvas.glcd->rect;

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    glcdSetXY(rect.w / 2, (rect.h - canvas.glcd->font->chars[0].image->height) / 2);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(labelsGet(label));
}

void canvasShowTune(bool clear, Tune *tune)
{
    tuneDraw(clear, tune, &canvas.layout->tune);
}


void canvasShowTest(bool clear)
{
}

void canvasDebugFPS(void)
{
    const Palette *pal = canvas.pal;

    glcdSetFont(&fontterminus12);
    glcdSetFontColor(pal->inactive);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    glcdSetXY(canvas.glcd->rect.w, 0);

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
    glcdWriteString(utilMkStr("%4d", oldFps));
}
