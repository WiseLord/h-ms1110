#include "spectrumview.h"

#include <string.h>

#include "gui/widget/spectrumcolumn.h"
#include "swtimers.h"

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

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol);

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

static bool checkSpectrumReady(void)
{
    if (swTimGet(SW_TIM_SP_CONVERT) == 0) {
        swTimSet(SW_TIM_SP_CONVERT, 20);
        return true;
    }

    return false;
}

static void calcSpCol(int16_t chan, int16_t scale, uint8_t col, SpectrumColumn *spCol)
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
}

void spViewDraw(bool clear, bool check, bool mirror, bool peaks, SpChan chan, GlcdRect *rect)
{
    if (clear) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(spData, 0, sizeof (SpData) * SP_CHAN_END);
    }

    if (check && !checkSpectrumReady()) {
        return;
    }

    if (chan == SP_CHAN_LEFT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_LEFT, spData[SP_CHAN_LEFT].raw, SPECTRUM_SIZE, fftGet43);
    }
    if (chan == SP_CHAN_RIGHT || chan == SP_CHAN_BOTH) {
        spGetADC(SP_CHAN_RIGHT, spData[SP_CHAN_RIGHT].raw, SPECTRUM_SIZE, fftGet43);
    }

    int16_t step = 6;                                     // Step of columns
    int16_t width = 4;                                    // Width of spectrum

    if (rect->w == 125) {
        step = 3;
        width = 2;
    }

    color_t *grad = NULL;

    glcdSetRect(rect);

    for (uint8_t col = 0; col < SPECTRUM_SIZE; col++) {
        int16_t x = col * step;

        SpectrumColumn spCol;
        calcSpCol(chan, rect->h, col, &spCol);
        if (!peaks) {
            spCol.peakW = 0;
        }
        GlcdRect colRect = {x, 0, width, rect->h};
        spectrumColumnDraw(&spCol, clear, &colRect, mirror, grad);
    }

    glcdResetRect();
}
