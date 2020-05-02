#include "setupview.h"

#include "display/glcd.h"
#include "gui/palette.h"
#include "rtc.h"
#include "setup.h"
#include "tr/labels.h"

static Setup old;

const char *getHeadLabel(SetupType type)
{
    Label label = LABEL_SETUP_MAIN;

    switch (type) {
    case SETUP_TIME:
        label = LABEL_SETUP_TIME;
        break;
    case SETUP_DATE:
        label = LABEL_SETUP_DATE;
        break;
    case SETUP_ALARM:
        label = LABEL_SETUP_ALARM;
        break;
    case SETUP_REMOTE:
        label = LABEL_SETUP_REMOTE;
        break;
    }

    return labelsGet(label);
}


static void drawHead(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    if (setup->active != old.active) {
        clear = true;
    }

    if (clear) {
        glcdDrawRect(0, 0, rect.w, 32, pal->bg);

        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(0, 0);
        glcdWriteString(getHeadLabel(setup->active));
    }
}

static void drawActive(bool clear)
{
    if (clear) {
        const Palette *pal = paletteGet();
        GlcdRect rect = glcdGet()->rect;

        Setup *setup = setupGet();

        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(rect.w / 2, 32);
        glcdSetFontAlign(GLCD_ALIGN_CENTER);
        glcdWriteString(getHeadLabel(setup->child));
    }
}

static void drawTm(const char *tm, bool select)
{
    const Palette *pal = paletteGet();

    if (select) {
        glcdSetFontColor(pal->bg);
        glcdSetFontBgColor(pal->fg);
    }

    glcdWriteString(tm);

    glcdSetFontColor(pal->fg);
    glcdSetFontBgColor(pal->bg);
}

static void drawTime(bool clear)
{
    (void)clear;

    const Palette *pal = paletteGet();

    Setup *setup = setupGet();

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    glcdSetXY(32, 32);
    glcdSetStringFramed(true);
    drawTm(utilMkStr("%02d", rtc.hour), setup->child == SETUP_TIME_HOUR);
    glcdWriteString(":");
    drawTm(utilMkStr("%02d", rtc.min), setup->child == SETUP_TIME_MINUTE);
    glcdWriteString(":");
    drawTm(utilMkStr("%02d", rtc.sec), setup->child == SETUP_TIME_SECOND);
    glcdSetStringFramed(false);
}


static void drawDate(bool clear)
{
    (void)clear;

    const Palette *pal = paletteGet();

    Setup *setup = setupGet();

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    glcdSetXY(32, 32);
    glcdSetStringFramed(true);
    drawTm(utilMkStr("%02d", rtc.date), setup->child == SETUP_DATE_DAY);
    glcdWriteString(":");
    drawTm(utilMkStr("%02d", rtc.month), setup->child == SETUP_DATE_MONTH);
    glcdWriteString(":");
    drawTm(utilMkStr("20%02d", rtc.year), setup->child == SETUP_DATE_YEAR);
    glcdSetStringFramed(false);
}

static void drawChild(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    if (setup->child != old.child) {
        clear = true;
    }

    if (clear) {
        const int16_t marginX = 16;
        glcdDrawRect(marginX, 32, rect.w - marginX * 2, 32, pal->bg);
    }

    switch (setup->active) {
    case SETUP_MAIN:
        drawActive(clear);
        break;
    case SETUP_TIME:
        drawTime(clear);
        break;
    case SETUP_DATE:
        drawDate(clear);
        break;
    }
}

static void drawArrows(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    if (clear) {
        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(0, 32);
        glcdSetFontAlign(GLCD_ALIGN_LEFT);
        glcdWriteString("<");

        glcdSetXY(rect.w, 32);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdWriteString(">");
    }
}


void setupViewDraw(bool clear)
{
    drawHead(clear);
    drawChild(clear);
    drawArrows(clear);

    Setup *setup = setupGet();
    old = *setup;
}
