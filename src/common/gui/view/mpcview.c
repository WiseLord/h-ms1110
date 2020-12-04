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

static void mpcViewCalcNameScroll(MpcView *this, int16_t max_oft)
{
    if (this->scroll_oft >= 0 && !this->scroll_pause) {
        this->scroll_oft = 0;
        this->scroll_left = true;
        this->scroll_pause = 20;
    }

    if (this->scroll_oft <= -max_oft && !this->scroll_pause) {
        this->scroll_oft = -max_oft;
        this->scroll_left = false;
        this->scroll_pause = 20;
    }

    if (this->scroll_pause > 0) {
        this->scroll_pause--;
    }

    if (0 == this->scroll_pause) {
        if (this->scroll_left) {
            this->scroll_oft--;
        } else {
            this->scroll_oft++;
        }
    }
}

static void mpcViewResetNameScroll(MpcView *this)
{
    this->scroll_oft = 0;
    this->scroll_left = true;
    this->scroll_pause = false;
}

static void mpcViewDrawName(MpcView *this, bool clear, const GlcdRect *rect)
{
    glcdSetRect(rect);

    if (clear) {
        mpcViewResetNameScroll(this);
    }

    const Palette *pal = paletteGet();

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    int16_t len = glcdCalcStringLen(this->meta);

    int16_t max_oft = len - rect->w;

    if (max_oft <= 0) {
        glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);
        mpcViewResetNameScroll(this);
    } else {
        if (this->scroll_event) {
            this->scroll_event = false;
            mpcViewCalcNameScroll(this, max_oft);
        }
    }

    glcdSetXY(this->scroll_oft, 0);
    glcdWriteString(this->meta);

    glcdResetRect();
}

static void mpcViewDrawTime(MpcView *this, const GlcdRect *rect)
{
    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    int16_t time = this->elapsed;

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

static void mpcViewDrawProgress(MpcView *this, const GlcdRect *rect)
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
    bar.value = this->elapsed;
    bar.min = 0;
    bar.max = this->duration;

    progressBarDraw(true, &bar);

    glcdResetRect();
}

void mpcViewDraw(MpcView *this, bool clear)
{
    if (clear) {
        mpcViewDrawIcon(&iconRect);
    }
    mpcViewDrawName(this, clear, &metaRect);
    mpcViewDrawTime(this, &timeRect);
    mpcViewDrawProgress(this, &progressRect);
}
