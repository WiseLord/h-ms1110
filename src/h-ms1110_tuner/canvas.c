#include "gui/canvas.h"

#include "amp.h"
#include "display/glcd.h"
#include "input.h"
#include "menu.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"

#include <stdlib.h>
#include <string.h>

#define SPECTRUM_SIZE   43

typedef union {
    struct {
        uint8_t show[SP_CHAN_END];   // Value to show
        uint8_t prev[SP_CHAN_END];   // Previous value
        uint8_t peak[SP_CHAN_END];   // Peak value
        uint8_t fall[SP_CHAN_END];   // Fall speed
    };
    SpectrumColumn col;
} SpCol;

typedef struct {
    SpCol col[SPECTRUM_SIZE];
} SpDrawData;

typedef struct {
    uint8_t raw[SPECTRUM_SIZE];
} SpData;


static void drawMenuItem(uint8_t idx, const tFont *fontItem);
static uint8_t calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol);
static void drawSpectrum(bool clear, bool check, SpChan chan, GlcdRect *rect);


static SpDrawData spDrawData;
static SpData spData[SP_CHAN_END];


// First/last indexes of spectrum indexes moving to output
static const int16_t spIdx[SPECTRUM_SIZE + 1] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,  9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
    20,  21,  23,  27,  31,  36,  42,  49,  57,  66,
    76,  88,  102, 119, 137, 159, 184, 213, 246, 285,
    330, 382, 441, 512,
};

static uint8_t calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol)
{
    int16_t raw;

    SpCol *spDrawCol = &spDrawData.col[col];

    if (chan == SP_CHAN_BOTH) {
        uint8_t rawL = spData[SP_CHAN_LEFT].raw[col];
        uint8_t rawR = spData[SP_CHAN_RIGHT].raw[col];
        if (rawL > rawR) {
            raw = rawL;
        } else {
            raw = rawR;
        }
        *spCol = spDrawCol->col;
    } else {
        raw = spData[chan].raw[col];
        spCol->showW = spDrawCol->show[chan];
        spCol->prevW = spDrawCol->prev[chan];
        spCol->peakW = spDrawCol->peak[chan];
        spCol->fallW = spDrawCol->fall[chan];
    }

    raw = raw * scale / N_DB;

    spCol->prevW = spCol->showW;
    if (raw < spCol->showW) {
        if (spCol->showW >= spCol->fallW) {
            spCol->showW -= spCol->fallW;
            spCol->fallW++;
        } else {
            spCol->showW = 0;
        }
    }

    if (raw > spCol->showW) {
        spCol->showW = raw;
        spCol->fallW = 1;
    }

    if (spCol->peakW <= raw) {
        spCol->peakW = raw + 1;
    } else {
        if (spCol->peakW && spCol->peakW > spCol->showW + 1) {
            spCol->peakW--;
        }
    }

    if (chan == SP_CHAN_BOTH) {
        spDrawCol->col = *spCol;
    } else {
        spDrawCol->show[chan] = (uint8_t)spCol->showW;
        spDrawCol->prev[chan] = (uint8_t)spCol->prevW;
        spDrawCol->peak[chan] = (uint8_t)spCol->peakW;
        spDrawCol->fall[chan] = (uint8_t)spCol->fallW;
    }

    return (uint8_t)raw;
}

static bool checkSpectrumReady(void)
{
    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 20);
        return true;
    }

    return false;
}

static void fftGet43(FftSample *sp, uint8_t *out, size_t size)
{
    for (size_t s = 0; s < size; s++) {
        int32_t max = 0;
        for (int16_t i = spIdx[s]; i < spIdx[s + 1]; i++) {
            int32_t calc = sp[i].fr * sp[i].fr + sp[i].fi * sp[i].fi;
            if (calc > max) {
                max = calc;
            }
        }
        uint16_t ret = (uint16_t)(max >> 15);
        out[s] = spGetDb(ret, 0, N_DB - 1);
    }
}

static void drawSpectrum(bool clear, bool check, SpChan chan, GlcdRect *rect)
{
    Spectrum *sp = spGet();

    if (check && !checkSpectrumReady()) {
        return;
    }

    if (chan == SP_CHAN_LEFT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet43);
    }
    if (chan == SP_CHAN_RIGHT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet43);
    }

    const int16_t step = 6;             // Step of columns
    const int16_t colW = 4;             // Column width

    const int16_t height = rect->h;     // Height of spectrum

    const int16_t y = rect->y;

    if (clear) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(spData, 0, sizeof (SpData) * SP_CHAN_END);
    }

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        int16_t x = col * step;

        SpectrumColumn spCol;
        calcSpCol(chan, height, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        GlcdRect rect = {x, y, colW, height};
        spectrumColumnDraw(&spCol, &rect, clear, NULL);
    }
}

void canvasShowSpectrum(bool clear)
{
    Spectrum *sp = spGet();
    GlcdRect rect = canvasGet()->glcd->rect;

    switch (sp->mode) {
    default:
        drawSpectrum(clear, true, SP_CHAN_RIGHT, &rect);
    }
}

void canvasShowStandby(bool clear)
{
    canvasShowTime(clear);
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
    const Layout *lt = canvasGet()->layout;

    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = lt->rect.w;
    MenuIdx menuIdx = menu->list[idx];
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = lt->rect.h - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? canvasGet()->pal->fg : canvasGet()->pal->bg);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(canvasGet()->pal->fg);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }

    glcdWriteString(menuGetName(menuIdx));

    // Draw menu value
    int16_t x = canvasGet()->glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    // Inverse value color if selected
    color_t color = canvasGet()->glcd->fontFg;
    color_t bgColor = canvasGet()->glcd->fontBg;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    glcdSetStringFramed(true);
    uint16_t strLen = glcdWriteString(menuGetValueStr(menuIdx));
    glcdSetStringFramed(false);

    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvasGet()->pal->bg);
}

void canvasShowMenu(bool clear)
{
    (void)clear;

    const Layout *lt = canvasGet()->layout;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvasGet()->pal->fg);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvasGet()->glcd->x, canvasGet()->glcd->y, lt->rect.w - canvasGet()->glcd->x, fHh, canvasGet()->pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvasGet()->glcd->fontFg);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}
