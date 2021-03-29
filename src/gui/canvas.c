#include "canvas.h"

#include <stdio.h>
#include <string.h>

#include "audio/audio.h"
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
    static DateTimeView view;

    RTC_type rtc;
    rtcGetTime(&rtc);

    view.hour = rtc.hour;
    view.min = rtc.min;
    view.sec = rtc.sec;
    view.month = rtc.month;
    view.year = rtc.year;
    view.date = rtc.date;
    view.wday = rtc.wday;

    view.mode = mode;

    dateTimeViewDraw(&view, clear);
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
    static InputView view;

    Amp *amp = ampGet();

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

    AudioProc *aProc = audioGet();
    if (aProc->par.flags & AUDIO_FLAG_MUTE) {
        canvasShowMute(clear);
        return;
    }

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

void canvasShowMute(bool clear)
{
    if (clear) {
        const Palette *pal = paletteGet();
        const tImage *img = iconFind(ICON_MUTE, &icons_hms1110);

        glcdSetXY(canvas.glcd->rect.w / 2 - img->width / 2, 0);
        glcdDrawImage(img, pal->fg, pal->bg);
    }
}

void canvasShowTune(bool clear, AudioTune tune)
{
    static TuneView view;

    AudioProc *aProc = audioGet();

    const AudioGrid *grid = aProc->par.grid[tune];
    int16_t value = aProc->par.tune[tune];
    int8_t step = (grid != NULL) ? grid->mStep : 0;

    if (view.value != value) {
        view.flags |= TUNE_FLAG_VALUE;
    }

    view.value = value;
    view.min = (grid != NULL) ? grid->min : 0;
    view.max = (grid != NULL) ? grid->max : 0;

    if (grid != NULL && grid->array != NULL) {
        value = grid->array[value];
    }

    if (tune == AUDIO_TUNE_GAIN) {
        view.name = labelsGet(LABEL_IN_TUNER + ampGet()->inType);
    } else {
        view.name = labelsGet(LABEL_VOLUME + (tune - AUDIO_TUNE_VOLUME));
    }

    switch (tune) {
    case AUDIO_TUNE_BASS_FREQ:
    case AUDIO_TUNE_SUB_CUT_FREQ:
    case AUDIO_TUNE_LOUD_PEAK_FREQ:
        value = value / STEP_MULT;
        snprintf(view.valStr, sizeof(view.valStr), "%4d", value);
        snprintf(view.unitStr, sizeof(view.unitStr), "\u2008%s", labelsGet(LABEL_HZ));
        break;
    case AUDIO_TUNE_MIDDLE_KFREQ:
    case AUDIO_TUNE_TREBLE_KFREQ:
        value = value * 1000 / STEP_MULT;
        snprintf(view.valStr, sizeof(view.valStr), "%3d.%1d", value / 1000, value % 1000 / 100);
        snprintf(view.unitStr, sizeof(view.unitStr), "\u2008%s%s", labelsGet(LABEL_K), labelsGet(LABEL_HZ));
        break;
    case AUDIO_TUNE_BASS_QUAL:
    case AUDIO_TUNE_MIDDLE_QUAL:
        value = value * 1000 / STEP_MULT;
        snprintf(view.valStr, sizeof(view.valStr), "%d.%02d", value / 1000, value % 1000 / 10);
        view.unitStr[0] = '\0';
        break;
    default:
        value = value * step / STEP_MULT;
        snprintf(view.valStr, sizeof(view.valStr), "%3d", value);
        snprintf(view.unitStr, sizeof(view.unitStr), "\u2008%s", labelsGet(LABEL_DB));
    }

    tuneViewDraw(&view, clear);
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
