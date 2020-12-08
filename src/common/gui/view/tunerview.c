#include "tunerview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/progressbar.h"

static const GlcdRect rectFreq = {44, 24, 199, 15};
static const GlcdRect rectMeta = {44, 41, 212, 14};
static const GlcdRect rectScale = {44, 58, 212, 6};
static const GlcdRect rectFav = {246, 24, 10, 15};

static void drawMeta(TunerView *this, bool clear)
{
//    if (this->mpc->flags & (MPC_FLAG_UPDATE_META | MPC_FLAG_UPDATE_STATUS)) {
    clear = true;
//    }

    const Palette *pal = paletteGet();
    const GlcdRect *rect = &rectMeta;

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    char meta[32];
    snprintf(meta, sizeof(meta), "%d-%d, 0x%04x",
             this->sync.band.fMin / 100, this->sync.band.fMax / 100,
             this->sync.tFlags);

    int16_t len = glcdCalcStringLen(meta);

    glcdSetRect(rect);

    glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);

    if (clear) {
        glcdSetXY(0, 0);
        glcdWriteString(meta);
    }

    glcdResetRect();
}

static void drawFreqValue(TunerView *this, bool clear)
{
    if (this->sync.flags & TUNERSYNC_FLAG_FREQ) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    uint16_t freq = this->sync.freq;
    uint8_t freqM = freq / 100;
    uint8_t freqK = freq % 100;

    char buf[32];
    snprintf(buf, sizeof(buf), "FM %3d.%02d", freqM, freqK);

    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(pal->active);

    glcdSetRect(&rectFreq);

    glcdSetXY(0, -4);
    glcdWriteString(buf);

    glcdResetRect();
}

static void drawProgress(TunerView *this, bool clear)
{
    if (this->sync.flags & (TUNERSYNC_FLAG_FREQ | TUNERSYNC_FLAG_BAND)) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();
    const GlcdRect *rect = &rectScale;

    ProgressBar bar;

    bar.lt.rect.x = 0;
    bar.lt.rect.y = 0;
    bar.lt.rect.w = rect->w;
    bar.lt.rect.h = rect->h;
    bar.lt.frame_width = 1;
    bar.lt.mark_count = 208;
    bar.lt.mark_width = 1;
    bar.value = this->sync.freq;
    bar.min = this->sync.band.fMin;
    bar.max = this->sync.band.fMax;
    bar.bgColor = pal->bg;

    glcdSetRect(rect);

    progressBarDraw(true, &bar);

    glcdResetRect();
}

static void drawFavNum(TunerView *this, bool clear)
{
    if (this->sync.flags & TUNERSYNC_FLAG_FAVS) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(pal->active);

    char buf[22];

    bool favFound = false;
    for (uint16_t i = 0; i < 9; i++) {
        if ((1 << i) & this->sync.favMask) {
            snprintf(buf, sizeof(buf), "%d", i);
            favFound = true;
            break;
        }
    }
    if (!favFound) {
        snprintf(buf, sizeof(buf), "-");
    }

    glcdSetRect(&rectFav);

    glcdSetXY(0, -4);
    glcdWriteString(buf);

    glcdResetRect();
}

void tunerViewDraw(TunerView *this, bool clear)
{
    drawFreqValue(this, clear);
    drawMeta(this, clear);
    drawProgress(this, clear);
    drawFavNum(this, clear);

    this->sync.flags = 0;
}
