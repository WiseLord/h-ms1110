#include "mpcview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/progressbar.h"

static const GlcdRect rectIconMpc = {0, 24, 40, 40};

static const GlcdRect rectIconStatus = {44, 24, 15, 15};

static const GlcdRect rectIconMedia = {68, 24, 15, 15};

static const GlcdRect rectIconRepeat = {92, 24, 15, 15};
static const GlcdRect rectIconSingle = {112, 24, 15, 15};
static const GlcdRect rectIconRandom = {132, 24, 15, 15};
static const GlcdRect rectIconConsume = {152, 24, 15, 15};

static const GlcdRect rectElapsed = {178, 24, 78, 15};
static const GlcdRect rectMeta = {44, 41, 212, 14};
static const GlcdRect rectProgress = {44, 58, 212, 6};

static void drawMpcIcon(MpcView *this, bool clear)
{
    (void)this;

    if (!clear) {
        return;
    }

    glcdSetRect(&rectIconMpc);

    const Palette *pal = paletteGet();

    const tImage *icon = iconFind(ICON_MPD, &icons_hms1110);
    glcdSetXY(0, 0);
    glcdDrawImage(icon, pal->fg, pal->bg);

    glcdResetRect();
}

static void drawStatusIcon(Icon icon, const GlcdRect *rect, color_t color)
{
    const Palette *pal = paletteGet();
    const tImage *img = iconFind(icon, &icons_hms1110);

    glcdSetRect(rect);
    glcdSetXY(0, 0);
    glcdDrawImage(img, color, pal->bg);
    glcdResetRect();
}

static void drawStatusIcons(MpcView *this, bool clear)
{
    if (this->mpc->flags & (MPC_FLAG_UPDATE_STATUS | MPC_FLAG_UPDATE_DURATION)) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();
    MpcStatus st = this->mpc->status;
    int32_t duration = this->mpc->duration;

    drawStatusIcon(st & MPC_PAUSED ? ICON_PAUSED : st & MPC_PLAYING ? ICON_PLAYING : ICON_STOPPED,
                   &rectIconStatus, pal->fg);
    drawStatusIcon(ICON_REPEAT, &rectIconRepeat, st & MPC_REPEAT ? pal->fg : pal->inactive);
    drawStatusIcon(ICON_SINGLE, &rectIconSingle, st & MPC_SINGLE ? pal->fg : pal->inactive);
    drawStatusIcon(ICON_RANDOM, &rectIconRandom, st & MPC_RANDOM ? pal->fg : pal->inactive);
    drawStatusIcon(ICON_CONSUME, &rectIconConsume, st & MPC_CONSUME ? pal->fg : pal->inactive);
    drawStatusIcon(st & MPC_PLAYING ? duration ? ICON_FILE : ICON_STREAM : ICON_IDLE,
                   &rectIconMedia, pal->fg);
}

static void calcNameScroll(MpcView *this, int16_t max_oft)
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

static void resetNameScroll(MpcView *this)
{
    this->scroll.oft = 0;
    this->scroll.left = true;
    this->scroll.pause = false;
    this->scroll.event = false;
}

static void drawMeta(MpcView *this, bool clear)
{
    if (this->mpc->flags & (MPC_FLAG_UPDATE_META | MPC_FLAG_UPDATE_STATUS)) {
        clear = true;
    }

    if (clear) {
        resetNameScroll(this);
    }

    const Palette *pal = paletteGet();
    const GlcdRect *rect = &rectMeta;

    glcdSetRect(rect);

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    const char *meta = this->mpc->status & (MPC_PLAYING) ? this->mpc->meta : "";

    int16_t len = glcdCalcStringLen(meta);

    int16_t max_oft = len - rect->w;

    if (max_oft <= 0) {
        glcdDrawRect(len, 0, rect->w - len, rect->h, pal->bg);
        resetNameScroll(this);
    } else {
        if (this->scroll.event) {
            clear = true;
            this->scroll.event = false;
            calcNameScroll(this, max_oft);
        }
    }

    if (clear) {
        glcdSetXY(this->scroll.oft, 0);
        glcdWriteString(meta);
    }

    glcdResetRect();
}

static void drawElapsed(MpcView *this, bool clear)
{
    if (this->mpc->flags & MPC_FLAG_UPDATE_ELAPSED) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    glcdSetRect(&rectElapsed);

    int16_t time = this->mpc->elapsed;

    int8_t sec = time % 60;
    time /= 60;
    int8_t min = time % 60;
    time /= 60;
    int8_t hour = time % 24;
    time /= 24;

    char buf[16];
    if (!(this->mpc->status & MPC_PLAYING)) {
        snprintf(buf, sizeof(buf), "  \u2008 -:--");
    } else if (time > 0) {
        snprintf(buf, sizeof(buf), "%2d.%02d:%02d", time, hour, min);
    } else if (hour > 0) {
        snprintf(buf, sizeof(buf), "%2d:%02d:%02d", hour, min, sec);
    } else {
        snprintf(buf, sizeof(buf), "  \u2008%2d:%02d", min, sec);
    }

    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(pal->active);

    glcdSetXY(0, -4);
    glcdWriteString(buf);

    glcdResetRect();
}

static void drawProgress(MpcView *this, bool clear)
{
    if (this->mpc->flags & (MPC_FLAG_UPDATE_ELAPSED | MPC_FLAG_UPDATE_DURATION)) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    const GlcdRect *rect = &rectProgress;

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
    bar.bgColor = pal->bg;

    progressBarDraw(true, &bar);

    glcdResetRect();
}

void mpcViewDraw(MpcView *this, bool clear)
{
    drawMpcIcon(this, clear);
    drawStatusIcons(this, clear);
    drawMeta(this, clear);
    drawElapsed(this, clear);
    drawProgress(this, clear);

    this->mpc->flags = 0;
}
