#include "mpcview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/icons.h"
#include "gui/palette.h"
#include "gui/widget/iconimage.h"
#include "gui/widget/progressbar.h"

static const GlcdRect rectIconStatus = {44, 0, 15, 15};

static const GlcdRect rectIconMedia = {68, 0, 15, 15};

static const GlcdRect rectIconRepeat = {92, 0, 15, 15};
static const GlcdRect rectIconSingle = {112, 0, 15, 15};
static const GlcdRect rectIconRandom = {132, 0, 15, 15};
static const GlcdRect rectIconConsume = {152, 0, 15, 15};

static const GlcdRect rectElapsed = {178, 0, 78, 15};
static const GlcdRect rectMeta = {44, 17, 212, 14};
static const GlcdRect rectProgress = {44, 34, 212, 6};

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

    IconImage iconStatus = {
        .rect = &rectIconStatus,
        .color = pal->fg,
        .icon = st & MPC_PAUSED ? ICON_PAUSED : st & MPC_PLAYING ? ICON_PLAYING : ICON_STOPPED,
    };
    iconImageDraw(&iconStatus, clear);

    IconImage iconRepeat = {
        .rect = &rectIconRepeat,
        .color = st & MPC_REPEAT ? pal->fg : pal->inactive,
        .icon = ICON_REPEAT,
    };
    iconImageDraw(&iconRepeat, clear);

    IconImage iconSingle = {
        .rect = &rectIconSingle,
        .color = st & MPC_SINGLE ? pal->fg : pal->inactive,
        .icon = ICON_SINGLE,
    };
    iconImageDraw(&iconSingle, clear);

    IconImage iconRandom = {
        .rect = &rectIconRandom,
        .color = st & MPC_RANDOM ? pal->fg : pal->inactive,
        .icon = ICON_RANDOM,
    };
    iconImageDraw(&iconRandom, clear);

    IconImage iconConsume = {
        .rect = &rectIconConsume,
        .color = st & MPC_CONSUME ? pal->fg : pal->inactive,
        .icon = ICON_CONSUME,
    };
    iconImageDraw(&iconConsume, clear);

    IconImage iconMedia = {
        .rect = &rectIconMedia,
        .color = pal->fg,
        .icon = st & MPC_PLAYING ? duration ? ICON_FILE : ICON_STREAM : ICON_IDLE,
    };
    iconImageDraw(&iconMedia, clear);
}

static void drawMeta(MpcView *this, bool clear)
{
    if (this->mpc->flags & (MPC_FLAG_UPDATE_META | MPC_FLAG_UPDATE_STATUS)) {
        clear = true;
    }

    const Palette *pal = paletteGet();

    glcdSetFont(&fontterminus14b);
    glcdSetFontColor(pal->active);

    this->scroll.rect = &rectMeta;
//    this->scroll.text = this->mpc->status & (MPC_PLAYING) ? this->mpc->meta : "";
    this->scroll.text = this->mpc->meta;

    scrollTextDraw(&this->scroll, clear);
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


    int time = this->mpc->elapsed;

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

    glcdSetRect(&rectElapsed);

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

    glcdSetRect(rect);

    progressBarDraw(&bar, true);

    glcdResetRect();
}

void mpcViewDraw(MpcView *this, bool clear)
{
    drawStatusIcons(this, clear);
    drawMeta(this, clear);
    drawElapsed(this, clear);
    drawProgress(this, clear);

    this->mpc->flags = 0;
}