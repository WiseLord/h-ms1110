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
#include "view/inputview.h"
#include "view/mpcview.h"
#include "view/tunerview.h"
#include "widget/iconimage.h"

static Canvas canvas;
static const GlcdRect rectIconInput = {0, 24, 40, 40};

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

static void canvasShowInputTuner(bool clear)
{
    static TunerView view;
    TunerSync *sync = tunerSyncGet();

    view.sync = *sync;

    tunerViewDraw(&view, clear);
}

static void canvasShowInputMpc(bool clear)
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

static void canvasShowInputDefault(bool clear)
{
    Amp *amp = ampGet();

    Label label = LABEL_BOOL_OFF;

    if (amp->inType != IN_NULL) {
        label = LABEL_IN_TUNER + amp->inType;
    }

    static InputView view;
    view.name = labelsGet(label);

    inputViewDraw(&view, clear);
}

void canvasShowInput(bool clear)
{
    InputType inType = ampGet()->inType;

    const Palette *pal = paletteGet();

    IconImage iconInput = {
        .rect = &rectIconInput,
        .color = pal->fg,
        .icon = inType == IN_NULL ? ICON_EMPTY : ICON_TUNER + inType,
    };
    iconImageDraw(&iconInput, clear);

    switch (inType) {
    case IN_MPD:
        canvasShowInputMpc(clear);
        break;
    case IN_TUNER:
        canvasShowInputTuner(clear);
        break;
    default:
        canvasShowInputDefault(clear);
        break;
    }
}

void canvasShowTune(bool clear, TuneView *tune)
{
    tuneViewDraw(clear, tune);
}

void canvasShowSetup(bool clear)
{
    setupViewDraw(clear);
}

void canvasShowTuner(bool clear)
{
    Spectrum *sp = spGet();

    GlcdRect rectL = {0, 0, 125, 20};
    spViewDraw(clear, true, false, sp->peaks, SP_CHAN_LEFT, &rectL);
    GlcdRect rectR = {131, 0, 125, 20};
    spViewDraw(clear, false, false, sp->peaks, SP_CHAN_RIGHT, &rectR);

    const Palette *pal = paletteGet();

    IconImage iconInput = {
        .rect = &rectIconInput,
        .color = pal->fg,
        .icon = ICON_TUNER,
    };
    iconImageDraw(&iconInput, clear);

    static TunerView view;
    Tuner *tuner = tunerGet();

    view.sync.freq = tuner->status.freq;
    view.sync.tFlags = tuner->status.flags;
    view.sync.favMask = stationFavGetMask(tuner->status.freq);
    view.sync.band.fMin = tuner->par.fMin;
    view.sync.band.fMax = tuner->par.fMax;
    view.sync.rdsParser = rdsParserGet();

    tunerViewDraw(&view, true);
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
    } else {
    }

    char buf[16];

    glcdSetXY(canvas.glcd->rect.w, 52);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    snprintf(buf, sizeof(buf), "%0d", (int)oldFps);
    glcdWriteString(buf);
}
