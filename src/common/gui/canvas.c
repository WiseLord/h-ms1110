#include "canvas.h"

#include <string.h>

#include "swtimers.h"

#include "widget/spview.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit();

    canvas.glcd = glcdGet();
    canvas.layout = layoutGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    // Default font parameters
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(canvas.pal->fg);
    glcdSetFontBgColor(canvas.pal->bg);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);
}

Canvas *canvasGet(void)
{
    return &canvas;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, COLOR_BLACK);
    glcdShift(0);

    glcdSetFontColor(COLOR_WHITE);
    glcdSetFontBgColor(COLOR_BLACK);
}

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks)
{
    GlcdRect rect = glcdGet()->rect;

    switch (mode) {
    case SP_MODE_STEREO:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_MIRROR:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, true, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_ANTIMIRROR:
        rect.h = rect.h / 2;
        spViewDraw(clear, true, true, peaks, SP_CHAN_LEFT, &rect);
        rect.y += rect.h;
        spViewDraw(clear, false, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    case SP_MODE_MIXED:
        spViewDraw(clear, true, false, peaks, SP_CHAN_BOTH, &rect);
        break;
    case SP_MODE_LEFT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_LEFT, &rect);
        break;
    case SP_MODE_RIGHT:
        spViewDraw(clear, true, false, peaks, SP_CHAN_RIGHT, &rect);
        break;
    default:
        break;
    }
}

void canvasShowTune(bool clear, Tune *pview)
{
    tuneDraw(clear, pview, &canvas.layout->par);
}


void canvasDebugFPS(void)
{
    const Palette *pal = canvas.pal;

    glcdSetFont(&fontterminus12);
    glcdSetFontColor(pal->inactive);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    glcdSetXY(canvas.glcd->rect.w, 0);

    static int32_t oldCnt = 0;
    static int32_t oldFps = 0;
    static int32_t frames = 0;

    frames++;

    int32_t cnt = swTimGet(SW_TIM_SYSTEM);
    if (cnt - oldCnt > 500) {
        int32_t fps = frames * 1000 / (cnt - oldCnt);
        frames = 0;
        oldFps = fps;
        oldCnt = cnt;
    } else {
    }
    glcdWriteString(utilMkStr("%4d", oldFps));
}
