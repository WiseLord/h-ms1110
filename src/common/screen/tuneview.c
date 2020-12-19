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

    const tFont *labelFont = &fontterminus24b;
    const tFont *valueFont = &fontterminus24b;

    glcdSetFont(labelFont);
    glcdSetFontColor(pal->fg);
    glcdSetXY(0, -2);
    glcdWriteString(labelsGet(tune->label));

    glcdSetFont(valueFont);
    glcdSetFontColor(pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdSetXY(rect->w, -2);

    char buf[4];
    snprintf(buf, sizeof(buf), "%3d", tune->value);
    glcdWriteString(buf);

    ProgressBar bar;
    bar.lt.rect.x = 1;
    bar.lt.rect.y = 24;
    bar.lt.rect.w = rect->w - 2;
    bar.lt.rect.h = rect->h - bar.lt.rect.y;
    bar.lt.frame_width = 2;
    bar.lt.mark_count = 83;
    bar.lt.mark_width = 2;
    bar.value = tune->value;
    bar.min = tune->min;
    bar.max = tune->max;
    bar.bgColor = pal->inactive;

    progressBarDraw(&bar, clear);
}
