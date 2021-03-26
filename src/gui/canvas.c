#include "canvas.h"

#include <stdio.h>
#include <string.h>

#include "input/matrix.h"
#include "rtc.h"
#include "screen/spectrumview.h"
#include "screen/setupview.h"
#include "swtimers.h"

#include "view/inputview.h"
#include "view/mpcview.h"
#include "view/starsview.h"
#include "view/tunerview.h"
#include "widget/iconimage.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit(GLCD_LANDSCAPE);

    canvas.glcd = glcdGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet();

    // Default font parameters
    glcdSetFont(&fontterminus14b);
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
    case SP_MODE_INVERTED:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, true, peaks, SP_CHAN_LEFT, &rect);
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

void canvasShowStars(bool clear, int16_t offset)
{
    StarsView view;
    view.offset = offset;
    view.stars = spViewGetDrawData();

    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 15);
        starsView(&view, clear);
    }
}

void canvasShowDateTime(bool clear, DateTimeMode mode)
{
    static DateTimeView dtv;

    RTC_type rtc;
    rtcGetTime(&rtc);

    dtv.hour = rtc.hour;
    dtv.min = rtc.min;
    dtv.sec = rtc.sec;
    dtv.month = rtc.month;
    dtv.year = rtc.year;
    dtv.date = rtc.date;
    dtv.wday = rtc.wday;

    dtv.mode = mode;

    dateTimeViewDraw(&dtv, clear);
}

void canvasShowInputSpectrum(bool clear)
{
#if !defined(_MODULE_PLAYER)
    Spectrum *sp = spGet();

    bool peaks = ((sp->flags & SP_FLAG_PEAKS) == SP_FLAG_PEAKS);

    GlcdRect rectL = {0, 40, 125, 24};
    GlcdRect rectR = {131, 40, 125, 24};
    spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rectL);
    spViewDraw(false, false, false, peaks, SP_CHAN_RIGHT, &rectR);
#endif
}

void canvasShowInputTuner(bool clear)
{
    static TunerView view;

    view.sync = tunerSyncGet();

    tunerViewDraw(&view, clear);
}

void canvasShowInputMpc(bool clear)
{
    static MpcView view;

    view.mpc = mpcGet();

    if (swTimGet(SW_TIM_SCROLL) <= 0) {
        swTimSet(SW_TIM_SCROLL, 100);
        view.scroll.flags |= SCROLL_EVENT;
    } else {
        view.scroll.flags &= ~SCROLL_EVENT;
    }

    mpcViewDraw(&view, clear);
}

void canvasShowInputSelector(bool clear, int8_t inIdx, bool inIdxUp, const void *inMap)
{
    Amp *amp = ampGet();

    static InputView view;

    Label label = LABEL_BOOL_OFF;

    if (amp->inType != IN_NULL) {
        label = LABEL_IN_TUNER + amp->inType;
    }

    view.name = labelsGet(label);
    view.inIdx = inIdx;
    view.inIdxUp = inIdxUp;
    view.inMap = inMap;
    view.scrollTimer = swTimGet(SW_TIM_SCROLL);

    inputViewDraw(&view, clear);
}

void canvasShowInput(bool clear, InputType inType)
{
    canvasShowInputSpectrum(clear);

    switch (inType) {
    case IN_MPD:
        canvasShowInputMpc(clear);
        break;
    case IN_TUNER:
        canvasShowInputTuner(clear);
        break;
    default:
        canvasShowDateTime(clear, DT_MODE_TIME);
        break;
    }
}

void canvasShowTune(bool clear, TuneView *tune)
{
    tuneViewDraw(clear, tune);
}

void canvasShowSetup(bool clear)
{
    static SetupView view;

    view.setup = setupGet();

    setupViewDraw(&view, clear);
}

void canvasDebugFPS(void)
{
    return;

    const Palette *pal = canvas.pal;

    glcdSetFont(&fontterminus14b);
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
    }

    char buf[16];

    glcdSetXY(canvas.glcd->rect.w, 17);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    snprintf(buf, sizeof(buf), "%3d %4d", (int)oldFps, (int)swTimGet(SW_TIM_SCROLL));
    glcdWriteString(buf);
}
