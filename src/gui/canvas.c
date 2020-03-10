#include "canvas.h"

#include "../audio/audio.h"
#include "../display/glcd.h"
#include "../input.h"
#include "../menu.h"
#include "../spectrum.h"
#include "../tr/labels.h"

#include <stdlib.h>
#include <string.h>

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

static SpDrawData spDrawData;

static Canvas canvas;

void canvasInit()
{
    glcdInit(&canvas.glcd);

    canvas.layout = layoutGet();

    PalIdx palIdx = PAL_DEFAULT;//(PalIdx)settingsRead(PARAM_DISPLAY_PALETTE);
    paletteSetIndex(palIdx);
    canvas.pal = paletteGet(palIdx);

    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);

    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = 3/*canvas.layout->menu.itemCnt*/;
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

static uint8_t calcSpCol(Spectrum *sp, int16_t chan, int16_t scale, uint8_t col,
                         SpectrumColumn *spCol)
{
    int16_t raw;

    SpCol *spDrawCol = &spDrawData.col[col];

    if (chan == SP_CHAN_BOTH) {
        uint8_t rawL = sp->data[SP_CHAN_LEFT].raw[col];
        uint8_t rawR = sp->data[SP_CHAN_RIGHT].raw[col];
        if (rawL > rawR) {
            raw = rawL;
        } else {
            raw = rawR;
        }
        *spCol = spDrawCol->col;
    } else {
        raw = sp->data[chan].raw[col];
        spCol->showW = spDrawCol->show[chan];
        spCol->prevW = spDrawCol->prev[chan];
        spCol->peakW = spDrawCol->peak[chan];
        spCol->fallW = spDrawCol->fall[chan];
    }

    raw = (scale * raw) >> 8; // / N_DB = 256

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

static void drawSpectrum(Spectrum *sp, SpChan chan, GlcdRect *rect)
{
    const int16_t step = /*(rect->w  + 1) / SPECTRUM_SIZE + 1*/6;    // Step of columns
    const int16_t colW = /*step - (step / 2)*/4;                     // Column width
    const int16_t num = (rect->w + colW - 1) / step;            // Number of columns

    const int16_t width = (num - 1) * step + colW;              // Width of spectrum
    const int16_t height = rect->h;                             // Height of spectrum

    const int16_t oft = (rect->w - width) / 2;                  // Spectrum offset for symmetry

    const int16_t y = rect->y;

    if (sp->redraw) {
        memset(&spDrawData, 0, sizeof (SpDrawData));
        memset(sp->data, 0, sizeof (SpData) * SP_CHAN_END);
    }

    for (uint8_t col = 0; col < num; col++) {
        int16_t x = oft + col * step;

        SpectrumColumn spCol;
        calcSpCol(sp, chan, height, col, &spCol);
        if (!sp->peaks) {
            spCol.peakW = 0;
        }
        GlcdRect rect = {x, y, colW, height};
        spectrumColumnDraw(&spCol, &rect, sp->redraw, NULL);
    }
}

void canvasShowSpectrum(bool clear)
{
    (void)clear;

    Spectrum *sp = spGet();
    GlcdRect rect = canvas.glcd->rect;

    if (!sp->ready) {
        return;
    }

    drawSpectrum(sp, SP_CHAN_BOTH, &rect);

    sp->redraw = false;
    sp->ready = false;
}

void canvasShowTime(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);

    glcdWriteString("Time");
}

void canvasShowStandby(bool clear)
{
    (void)clear;

    canvasShowSpectrum(clear);

//    return;
//    canvasShowTest1(clear);
}

void canvasShowTune(bool clear)
{
    const Layout *lt = canvas.layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
    }

    const int16_t value = aProc->par.tune[aProc->tune].value;

    const AudioGrid *grid = aProc->par.tune[aProc->tune].grid;
    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const uint8_t mStep = grid ? grid->mStep : 0;

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }

    // Bar
    StripedBar bar = {value, min, max};
    stripedBarDraw(&bar, &lt->tune.bar, clear);

    // Value
    glcdSetXY(lt->rect.w, lt->tune.valY);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdSetFont(lt->tune.valFont);
    glcdWriteString(utilMkStr("%3d", value * mStep / 8));
}

void canvasShowAudioInput(bool clear)
{
    const Layout *lt = canvas.layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvas.pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
    const Layout *lt = canvas.layout;

    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = lt->rect.w;
    MenuIdx menuIdx = menu->list[idx];
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = lt->rect.h - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? canvas.pal->fg : canvas.pal->bg);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(canvas.pal->fg);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }

    glcdWriteString(menuGetName(menuIdx));

    // Draw menu value
    int16_t x = canvas.glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    // Inverse value color if selected
    color_t color = canvas.glcd->fontFg;
    color_t bgColor = canvas.glcd->fontBg;
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
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvas.pal->bg);
}

void canvasShowMenu(bool clear)
{
    (void)clear;

    const Layout *lt = canvas.layout;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvas.pal->fg);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvas.glcd->x, canvas.glcd->y, lt->rect.w - canvas.glcd->x, fHh, canvas.pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvas.glcd->fontFg);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}

void canvasShowTest1(bool clear)
{
    (void)clear;
    const AudioGrid *grid = audioGet()->par.tune[AUDIO_TUNE_BASS].grid;

    glcdSetFont(&fontterminus12);

    InputCtx *inCtx = inputGetCtx();

    const int16_t sw = 210;
    const int16_t zoneCnt = grid->max - grid->min + 1;

    for (uint8_t i = AIN_POT_A; i < AIN_POT_END; i++) {
        int8_t pot = inputGetPot(i);
        int16_t len = (sw * (pot - grid->min) + sw / 2) / zoneCnt;

        int16_t xb = 0;
        int16_t yb = 16 * i;

        glcdDrawRect(xb, yb, len, 12, COLOR_WHITE);
        glcdDrawRect(xb + len, yb, sw - len, 12, COLOR_GRAY);
        glcdSetXY(xb + sw + 2, yb);
        glcdWriteString(utilMkStr("%2d.%4d", pot, inCtx->potData[i]));
        for (int16_t j = 1; j < zoneCnt; j++) {
            glcdDrawRect(xb + j * sw / zoneCnt, yb, 1, 12, RGB_CONV(0xA0A0A0));
        }
    }

    AnalogBtn aBtn = inputGetCtx()->aBtn;

    for (AnalogBtn i = 0; i < ABTN_END; i++) {
        glcdDrawCircle(16 + 20 * i, 40, 6, i == aBtn ? COLOR_WHITE : COLOR_GRAY);
    }

    glcdSetXY(200, 40);
    glcdWriteString(utilMkStr("0x%04x", inCtx->matrix));
}

void canvasShowTest2(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);
    glcdWriteString("Spectrum");

    glcdSetFont(&fontterminus16);

    static int16_t adcDataMin[AIN_END];
    static int16_t adcDataMax[AIN_END];

    InputCtx *inCtx = inputGetCtx();

    for (AnalogInput ain = AIN_POT_A; ain < AIN_END; ain++) {
        if (inCtx->adcData[ain] < adcDataMin[ain] || adcDataMin[ain] == 0) {
            adcDataMin[ain] = inCtx->adcData[ain];
        }
        if (inCtx->adcData[ain] > adcDataMax[ain] || adcDataMax[ain] == 0) {
            adcDataMax[ain] = inCtx->adcData[ain];
        }
    }

    glcdSetXY(0, 32);
    glcdWriteString(utilMkStr("%4d %4d", 3844 - inCtx->potData[0], 3844 - inCtx->potData[1]));

    glcdSetXY(0, 48);
    glcdWriteString(utilMkStr("%4d %4d", 3844 - inCtx->adcData[0], 3844 - inCtx->adcData[1]));

    glcdSetXY(150, 0);
    glcdWriteString(utilMkStr("%4d %4d %4d", inCtx->adcData[0], inCtx->adcData[1], inCtx->adcData[2]));

    glcdSetXY(150, 16);
    glcdWriteString(utilMkStr("%4d %4d %4d", adcDataMin[0], adcDataMin[1], adcDataMin[2]));

    glcdSetXY(150, 32);
    glcdWriteString(utilMkStr("%4d %4d %4d", adcDataMax[0], adcDataMax[1], adcDataMax[2]));

    glcdSetXY(150, 48);
    glcdWriteString(utilMkStr("%4d %4d %4d", inputGetPot(0), inputGetPot(1), inputGetPot(2)));
}
