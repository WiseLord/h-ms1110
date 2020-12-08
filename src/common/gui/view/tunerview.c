#include "tunerview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/progressbar.h"

static const GlcdRect rectFreq = {44, 24, 199, 15};
static const GlcdRect rectScale = {44, 58, 212, 6};

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
    snprintf(buf, sizeof(buf), "FM %3d.%02d - %d", freqM, freqK, this->sync.band.fMin);

    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(pal->active);

    glcdSetRect(&rectFreq);

    glcdSetXY(0, -4);
    glcdWriteString(buf);

    glcdResetRect();
}


static void drawProgress(TunerView *this, bool clear)
{
    if (this->sync.flags & TUNERSYNC_FLAG_FREQ ||
        this->sync.flags & TUNERSYNC_FLAG_BAND) {
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


void tunerViewDraw(TunerView *this, bool clear)
{
    drawFreqValue(this, clear);
    drawProgress(this, clear);

    this->sync.flags = 0;
}
