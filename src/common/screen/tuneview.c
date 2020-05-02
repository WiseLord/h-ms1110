#include "tuneview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"
#include "tr/labels.h"
#include "utils.h"
#include "gui/widget/progressbar.h"

void tuneViewDraw(bool clear, TuneView *tune)
{
    const Palette *pal = paletteGet();
    GlcdRect *rect = glcdGetRect();

    const tFont *labelFont = &fontterminus32;
    const tFont *valueFont = &fontterminus32;

    glcdSetFont(labelFont);
    glcdSetFontColor(pal->fg);
    glcdSetXY(0, 0);
    glcdWriteString(labelsGet(tune->label));

    glcdSetFont(valueFont);
    glcdSetFontColor(pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdSetXY(rect->w, 0);

    char buf[4];
    snprintf(buf, sizeof(buf), "%3d", tune->value);
    glcdWriteString(buf);

    ProgressBar bar;
    bar.lt.rect.x = 0;
    bar.lt.rect.y = 44;
    bar.lt.rect.w = rect->w;
    bar.lt.rect.h = rect->h - bar.lt.rect.y;
    bar.lt.fw = 3;
    bar.lt.sc = 83;
    bar.lt.sw = 2;
    bar.value = tune->value;
    bar.min = tune->min;
    bar.max = tune->max;

    progressBarDraw(clear, &bar);
}
