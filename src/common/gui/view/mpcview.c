#include "mpcview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/progressbar.h"

static const GlcdRect iconRect = {0, 24, 40, 40};
static const GlcdRect timeRect = {196, 24, 60, 14};
static const GlcdRect metaRect = {44, 42, 212, 14};
static const GlcdRect progressRect = {44, 58, 212, 6};

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
    if (this->scroll.oft >= 0 && !this->scroll.pause) {
        this->scroll.oft = 0;
        this->scroll.left = true;
        this->scroll.pause = 20;
    }

    if (this->scroll.oft <= -max_oft && !this->scroll.pause) {
        this->scroll.oft = -max_oft;
        this->scroll.left = false;
        this->scroll.pause = 20;
    }

    if (this->scroll.pause > 0) {
        this->scroll.pause--;
    }

    if (0 == this->scroll.pause) {
        if (this->scroll.left) {
            this->scroll.oft--;
        } else {
            this->scroll.oft++;
        }
    }
}

static void mpcViewResetNameScroll(MpcView *this)
{
    this->scroll.oft = 0;
    this->scroll.left = true;
    this->scroll.pause = false;
    this->scroll.event = false;
}

static void mpcViewDrawName(MpcView *this, bool clear, const GlcdRect *rect)
{
    if (this->mpc->flags & MPC_FLAG_UPDATE_META) {
        clear = true;
    }

    if (clear) {
        mpcViewResetNameScroll(this);
    }

    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    int16_t len = glcdCalcStringLen(this->mpc->meta);

    int16_t max_oft = len - rect->w;

    if (max_oft <= 0) {
        glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);
        mpcViewResetNameScroll(this);
    } else {
        if (this->scroll.event) {
            clear = true;
            this->scroll.event = false;
            mpcViewCalcNameScroll(this, max_oft);
        }
    }

    if (clear) {
        glcdSetXY(this->scroll.oft, 0);
        glcdWriteString(this->mpc->meta);
    }

    glcdResetRect();
}

static void mpcViewDrawTime(MpcView *this, bool clear, const GlcdRect *rect)
{
    if (this->mpc->flags & MPC_FLAG_UPDATE_ELAPSED) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    glcdSetRect(rect);

    const Palette *pal = paletteGet();

    int16_t time = this->mpc->elapsed;

    int8_t sec = time % 60;
    time /= 60;
    int8_t min = time % 60;
    time /= 60;
    int8_t hour = time % 24;
    time /= 24;

    char buf[16];
    if (time > 0) {
        snprintf(buf, sizeof(buf), "%2d.%02d:%02d", time, hour, min);
    } else if (hour > 0) {
        snprintf(buf, sizeof(buf), "%2d:%02d:%02d", hour, min, sec);
    } else {
        snprintf(buf, sizeof(buf), "  \u2008%2d:%02d", min, sec);
    }

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    glcdSetXY(0, 0);
    glcdWriteString(buf);

    glcdResetRect();
}

static void mpcViewDrawProgress(MpcView *this, bool clear, const GlcdRect *rect)
{
    if (this->mpc->flags & MPC_FLAG_UPDATE_ELAPSED ||
        this->mpc->flags & MPC_FLAG_UPDATE_DURATION) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    glcdSetRect(rect);

    ProgressBar bar;

    bar.lt.rect.x = 0;
    bar.lt.rect.y = 0;
    bar.lt.rect.w = rect->w;
    bar.lt.rect.h = rect->h;
    bar.lt.frame_width = 1;
    bar.lt.mark_count = 208;
    bar.lt.mark_width = 1;
    bar.value = this->mpc->elapsed;
    bar.min = 0;
    bar.max = this->mpc->duration;

    progressBarDraw(true, &bar);

    glcdResetRect();
}

void mpcViewDraw(MpcView *this, bool clear)
{
    if (clear) {
        mpcViewDrawIcon(&iconRect);
    }
    mpcViewDrawName(this, clear, &metaRect);
    mpcViewDrawTime(this, clear, &timeRect);
    mpcViewDrawProgress(this, clear, &progressRect);

    this->mpc->flags = 0;
}
