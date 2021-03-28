#include "tuneview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"
#include "tr/labels.h"
#include "utils.h"
#include "gui/widget/progressbar.h"

static void drawLabel(TuneView *this, bool clear)
{
    if (clear) {
        const Palette *pal = paletteGet();

        const tFont *labelFont = &fontterminus24b;

        glcdSetFont(labelFont);
        glcdSetFontColor(pal->fg);
        glcdSetXY(0, -2);
        glcdWriteString(this->name);
    }
}

static void drawValue(TuneView *this, bool clear)
{
    if (this->flags & TUNE_FLAG_VALUE) {
        clear = true;
    }

    if (clear) {
        const Palette *pal = paletteGet();

        const tFont *valFont = &fontterminus24b;
        const tFont *unitFont = &fontterminus14b;

        GlcdRect *rect = glcdGetRect();

        glcdSetFont(unitFont);
        glcdSetFontColor(pal->fg);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdSetXY(rect->w, 5);

        int16_t len = glcdWriteString(this->unitStr);

        glcdSetFont(valFont);
        glcdSetFontColor(pal->fg);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdSetXY(rect->w - len, -2);

        glcdWriteString(this->valStr);
    }
}

static void drawProgressBar(TuneView *this, bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect *rect = glcdGetRect();

    ProgressBar bar;
    bar.lt.rect.x = 1;
    bar.lt.rect.y = 24;
    bar.lt.rect.w = rect->w - 2;
    bar.lt.rect.h = rect->h - bar.lt.rect.y;
    bar.lt.frame_width = 2;
    bar.lt.mark_count = 83;
    bar.lt.mark_width = 2;
    bar.value = this->value;
    bar.min = this->min;
    bar.max = this->max;
    bar.bgColor = pal->inactive;

    progressBarDraw(&bar, clear);
}

void tuneViewDraw(TuneView *this, bool clear)
{
    drawLabel(this, clear);
    drawValue(this, clear);
    drawProgressBar(this, clear);

    this->flags = TUNE_FLAG_INIT;
}
