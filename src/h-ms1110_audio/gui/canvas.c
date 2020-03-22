#include "canvas.h"

#include "i2c.h"

#include "amp.h"
#include "audio/audio.h"
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

static Canvas canvas;


// First/last indexes of spectrum indexes moving to output
static const int16_t spIdx[SPECTRUM_SIZE + 1] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,  9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
    20,  21,  23,  27,  31,  36,  42,  49,  57,  66,
    76,  88,  102, 119, 137, 159, 184, 213, 246, 285,
    330, 382, 441, 512,
};

void canvasInit()
{
    glcdInit();

    canvas.glcd = glcdGet();
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
    GlcdRect rect = canvas.glcd->rect;

    switch (sp->mode) {
    default:
        drawSpectrum(clear, true, SP_CHAN_LEFT, &rect);
    }
}

static void drawTmSpacer(char spacer, bool clear)
{
    if (clear) {
        glcdWriteUChar(LETTER_SPACE_CHAR);
        glcdWriteUChar(spacer);
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }
}

static void drawTm(RTC_type *rtc, RtcMode tm, color_t fg)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(fg);
    glcdSetFontBgColor(canvas.pal->bg);
    if (rtc->etm == tm) {
        glcdSetFontColor(canvas.pal->bg);
        glcdSetFontBgColor(fg);
    }
    glcdWriteUChar(LETTER_SPACE_CHAR);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteUChar(LETTER_SPACE_CHAR);
    }
    if (time >= 0) {
        glcdWriteString(utilMkStr("%02d", time));
    } else {
        glcdWriteString("--");
    }
    glcdWriteUChar(LETTER_SPACE_CHAR);
    glcdSetFontColor(fg);
    glcdSetFontBgColor(canvas.pal->bg);
}

void canvasShowTime(bool clear)
{
//    (void)clear;

//    glcdSetFont(&fontterminus32);

//    glcdSetXY(0, 0);

//    glcdWriteString("Time");

    AmpStatus status = ampGet()->status;
    color_t fg = (status == AMP_STATUS_STBY ? canvas.pal->inactive : canvas.pal->fg);

    const Layout *lt = canvas.layout;

    RTC_type rtcStruct; // TODO: Use one struct in rtc driver
    rtcStruct.etm = rtcGetMode();

    RTC_type *rtc = &rtcStruct;

    rtcGetTime(rtc);

    // HH:MM:SS
    glcdSetFont(lt->time.hmsFont);

    int16_t digW = lt->time.hmsFont->chars[glcdFontSymbolPos('0')].image->width;
    int16_t ltspW = lt->time.hmsFont->chars[glcdFontSymbolPos(LETTER_SPACE_CHAR)].image->width;

    int16_t timeLen = 6 * digW + 15 * ltspW;    // 6 digits HHMMSS + 13 letter spaces + 2 ':'
    int16_t timeX = (lt->rect.w - (timeLen)) / 2;

    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_HOUR, fg);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_MIN, fg);

    drawTmSpacer(':', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.hmsY);
    drawTm(rtc, RTC_SEC, fg);

    // DD:MM:YYYY
    glcdSetFont(lt->time.dmyFont);

    digW = lt->time.dmyFont->chars[glcdFontSymbolPos('0')].image->width;
    ltspW = lt->time.dmyFont->chars[glcdFontSymbolPos(LETTER_SPACE_CHAR)].image->width;
    timeLen = 8 * digW + 17 * ltspW;            // 8 digits HHMMSS + 15 letter spaces + 2 '.'

    timeX = (lt->rect.w - timeLen) / 2;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_DATE, fg);

    drawTmSpacer('.', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_MONTH, fg);

    drawTmSpacer('.', clear);

    timeX += digW * 2 + ltspW * 6;
    glcdSetXY(timeX, lt->time.dmyY);
    drawTm(rtc, RTC_YEAR, fg);

    // Weekday
    glcdSetFont(lt->time.wdFont);
    glcdSetFontColor(fg);

    // Clear the area with weekday label
    glcdDrawRect(0, lt->time.wdY, lt->rect.w,
                 (int16_t)lt->time.wdFont->chars[0].image->height, canvas.pal->bg);

    const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + rtc->wday));
    glcdSetXY(lt->rect.w / 2, lt->time.wdY);
    glcdSetFontAlign(GLCD_ALIGN_CENTER);
    glcdWriteString(wdayLabel);
}

void canvasShowStandby(bool clear)
{
    canvasShowTime(clear);
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
    const int8_t mStep = grid ? grid->mStep : 0;

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
