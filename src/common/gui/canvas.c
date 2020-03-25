#include "canvas.h"

#include <string.h>

#include "amp.h"
#include "display/glcd.h"
#include "menu.h"
#include "rtc.h"
#include "spectrum.h"
#include "swtimers.h"

#define SPECTRUM_SIZE   43

typedef union {
    struct {
        uint8_t show[SP_CHAN_END];   // Value to show
        uint8_t prev[SP_CHAN_END];   // Previous value
        uint8_t peak[SP_CHAN_END];   // Peak value
        uint8_t fall[SP_CHAN_END];   // Fall speed
    };
    SpectrumColumn col;
} SpCol;

typedef struct {
    SpCol col[SPECTRUM_SIZE];
} SpDrawData;

typedef struct {
    uint8_t raw[SPECTRUM_SIZE];
} SpData;


static Canvas canvas;

static SpDrawData spDrawData;
static SpData spData[SP_CHAN_END];

// First/last indexes of spectrum indexes moving to output
static const int16_t spIdx[SPECTRUM_SIZE + 1] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,  9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
    20,  21,  23,  27,  31,  36,  42,  49,  57,  66,
    76,  88,  102, 119, 137, 159, 184, 213, 246, 285,
    330, 382, 441, 512,
};

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol);
static void drawSpectrum(bool clear, bool check, bool mirror, SpChan chan, GlcdRect *rect);
static void drawSpectrumMode(bool clear, GlcdRect rect);
static void drawWaterfall(bool clear);

void canvasInit()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);

    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = 3/*canvas.layout->menu.itemCnt*/;
}

void canvasInit_()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.glcd->rect = canvas.layout->rect;

    canvas.layout = layoutGet();
    menuGet()->dispSize = canvas.layout->menu.itemCnt;

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    // Default font parameters
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, COLOR_BLACK);
    glcdShift(0);

    glcdSetFontColor(COLOR_WHITE);
    glcdSetFontBgColor(COLOR_BLACK);
}


static void fftGet43(FftSample *sp, uint8_t *out, size_t size)
{
    for (size_t s = 0; s < size; s++) {
        int32_t max = 0;
        for (int16_t i = spIdx[s]; i < spIdx[s + 1]; i++) {
            int32_t calc = sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi;
            if (calc > max) {
                max = calc;
            }
        }
        uint16_t ret = (uint16_t)(max >> 15);
        out[s] = spGetDb(ret, 0, N_DB - 1);
    }
}

static bool checkSpectrumReady(void)
{
    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 20);
        return true;
    }

    return false;
}

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol)
{
    int16_t raw;

    SpCol *spDrawCol = &spDrawData.col[col];

    if (chan == SP_CHAN_BOTH) {
        uint8_t rawL = spData[SP_CHAN_LEFT].raw[col];
        uint8_t rawR = spData[SP_CHAN_RIGHT].raw[col];
        if (rawL > rawR) {
            raw = rawL;
        } else {
            raw = rawR;
        }
        *spCol = spDrawCol->col;
    } else {
        raw = spData[chan].raw[col];
        spCol->showW = spDrawCol->show[chan];
        spCol->prevW = spDrawCol->prev[chan];
        spCol->peakW = spDrawCol->peak[chan];
        spCol->fallW = spDrawCol->fall[chan];
    }

    raw = raw * scale / N_DB;

    spCol->prevW = spCol->showW;
    if (raw < spCol->showW) {
        if (spCol->showW >= spCol->fallW) {
            spCol->showW -= spCol->fallW;
            spCol->fallW++;
        } else {
            spCol->showW = 0;
        }
    }

    if (raw > spCol->showW) {
        spCol->showW = raw;
        spCol->fallW = 1;
    }

    if (spCol->peakW <= raw) {
        spCol->peakW = raw + 1;
    } else {
        if (spCol->peakW && spCol->peakW > spCol->showW + 1) {
            spCol->peakW--;
        }
    }

    if (chan == SP_CHAN_BOTH) {
        spDrawCol->col = *spCol;
    } else {
        spDrawCol->show[chan] = (uint8_t)spCol->showW;
        spDrawCol->prev[chan] = (uint8_t)spCol->prevW;
        spDrawCol->peak[chan] = (uint8_t)spCol->peakW;
        spDrawCol->fall[chan] = (uint8_t)spCol->fallW;
    }
}

static void drawSpectrum(bool clear, bool check, bool mirror, SpChan chan, GlcdRect *rect)
{
    if (check && !checkSpectrumReady()) {
        return;
    }

    if (chan == SP_CHAN_LEFT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet43);
    }
    if (chan == SP_CHAN_RIGHT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet43);
    }

    const int16_t step = 6;                                     // Step of columns
    const int16_t width = 4;                                    // Width of spectrum

    if (clear) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(spData, 0, sizeof (SpData) * SP_CHAN_END);
    }

    Spectrum *sp = spGet();

    color_t *grad = NULL;

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        int16_t x = col * step;

        SpectrumColumn spCol;
        calcSpCol(chan, rect->h, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        GlcdRect colRect = {x, rect->y, width, rect->h};
        spectrumColumnDraw(&spCol, &colRect, clear, mirror, grad);
    }
}

static void drawSpectrumMode(bool clear, GlcdRect rect)
{
    Spectrum *sp = spGet();

    switch (sp->mode) {
    case SP_MODE_STEREO:
    case SP_MODE_MIRROR:
    case SP_MODE_ANTIMIRROR:
        rect.h = rect.h / 2;
        drawSpectrum(clear, true, sp->mode == SP_MODE_ANTIMIRROR, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        drawSpectrum(clear, false, sp->mode == SP_MODE_MIRROR, SP_CHAN_RIGHT, &rect);
        break;
    default:
        drawSpectrum(clear, true, false, SP_CHAN_BOTH, &rect);
        break;
    }
}

static void drawWaterfall(bool clear)
{
    (void)clear;
}

void canvasShowSpectrum(bool clear)
{
    Spectrum *sp = spGet();

    switch (sp->mode) {
    case SP_MODE_STEREO:
    case SP_MODE_MIRROR:
    case SP_MODE_ANTIMIRROR:
    case SP_MODE_MIXED:
        drawSpectrumMode(clear, glcdGet()->rect);
        break;
    case SP_MODE_WATERFALL:
        drawWaterfall(clear);
        break;
    default:
        break;
    }
}

void canvasShowTime(bool clear)
{
    (void)clear;

    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());
    AmpStatus status = ampGet()->status;

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetXY(0, 0);
    glcdSetFont(lt->time.font);
    glcdSetFontColor(status == AMP_STATUS_STBY ? pal->inactive : pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_LEFT);

    glcdWriteString(utilMkStr("%02d:%02d:%02d", rtc.hour, rtc.min, rtc.sec));
}

void canvasShowDate(bool clear)
{
    (void)clear;

    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());
    AmpStatus status = ampGet()->status;

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetXY(0, 0);
    glcdSetFont(lt->time.font);
    glcdSetFontColor(status == AMP_STATUS_STBY ? pal->inactive : pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_LEFT);

    glcdWriteString(utilMkStr("%02d.%02d.%04d", rtc.date, rtc.month, 2000 + rtc.year));
}

void canvasShowStandby(bool clear, bool ymd)
{
    if (ymd) {
        canvasShowDate(clear);
    } else {
        canvasShowTime(clear);
    }
}

void canvasDebugFPS(void)
{
    const Layout *lt = layoutGet();
    const Palette *pal = paletteGet(paletteGetIndex());

    glcdSetFont(&fontterminus12);
    glcdSetFontColor(pal->inactive);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    glcdSetXY(lt->rect.w, 0);

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
