#include "mpcview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/progressbar.h"

static const GlcdRect iconRect = {0, 24, 40, 40};
static const GlcdRect timeRect = {218, 24, 38, 14};
static const GlcdRect metaRect = {44, 40, 212, 14};
static const GlcdRect progressRect = {44, 56, 212, 8};

static void mpcViewDrawIcon(const GlcdRect *rect)
{
    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    const tImage *icon = iconFind(ICON_MPD, &icons_hms1110);
    glcdSetXY(0, 0);
    glcdDrawImage(icon, pal->fg, pal->bg);

    glcdResetRect();
}

static void mpcViewDrawName(const GlcdRect *rect, const char *name)
{
    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    glcdSetFont(&fontterminus14);
    glcdSetFontColor(pal->active);

    glcdSetXY(0, 0);
    int16_t len = glcdWriteString(name);
    glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);

    glcdResetRect();
}

static void mpcViewDrawTime(const GlcdRect *rect, int16_t time)
{
    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    int8_t sec = time % 60;
    time /= 60;
    int8_t min = time % 60;
    time /= 60;
    int8_t hour = time % 24;

    char buf[8];
    if (hour > 0) {
        snprintf(buf, sizeof(buf), "%2d:%02d", hour, min);
    } else {
        snprintf(buf, sizeof(buf), "%2d:%02d", min, sec);
    }

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    glcdSetXY(0, 0);
    glcdWriteString(buf);

    glcdResetRect();
}

static void mpcViewDrawProgress(const GlcdRect *rect, int16_t time, int16_t duration)
{
    glcdSetRect(rect);

    ProgressBar bar;

    bar.lt.rect.x = 0;
    bar.lt.rect.y = 0;
    bar.lt.rect.w = rect->w;
    bar.lt.rect.h = rect->h;
    bar.lt.frame_width = 1;
    bar.lt.mark_count = 208;
    bar.lt.mark_width = 1;
    bar.value = time;
    bar.min = 0;
    bar.max = duration;

    progressBarDraw(true, &bar);

    glcdResetRect();
}

void mpcViewDraw(bool clear, MpcView *view)
{
    (void)clear;

    mpcViewDrawIcon(&iconRect);
    mpcViewDrawName(&metaRect, view->meta);
    mpcViewDrawTime(&timeRect, view->time);
    mpcViewDrawProgress(&progressRect, view->time, view->duration);
}
