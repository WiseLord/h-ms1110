#include "tune.h"

#include "display/glcd.h"
#include "gui/palette.h"
#include "utils.h"

void tuneDraw(bool clear, Tune *tune, LayoutTune *lt)
{
    const Palette *pal = paletteGet();
    GlcdRect *rect = glcdGetRect();

    glcdSetFont(lt->value.font);
    glcdSetFontColor(pal->fg);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdSetXY(rect->w, 0);

    glcdWriteString(utilMkStr("%d", tune->value));

    ProgressBar bar;
    bar.lt.rect.x = 0;
    bar.lt.rect.y = lt->bar.y;
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
