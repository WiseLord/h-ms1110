#include "setupview.h"

#include <stdio.h>

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

    glcdSetFontColor(select ? pal->bg : pal->fg);
    glcdSetFontBgColor(select ? pal->fg : pal->bg);

    glcdWriteString(tm);

    glcdSetFontColor(pal->fg);
    glcdSetFontBgColor(pal->bg);
}

static void drawTime(bool clear)
{
    (void)clear;

    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetFont(&fontterminus32);

    char hour[3], min[3], sec[3];
    int16_t len = 0;

    snprintf(hour, sizeof(hour), "%02d", rtc.hour);
    snprintf(min, sizeof(min), "%02d", rtc.min);
    snprintf(sec, sizeof(sec), "%02d", rtc.sec);

    glcdSetStringFramed(true);

    len += glcdCalcStringLen(hour);
    len += glcdCalcStringLen(":");
    len += glcdCalcStringLen(min);
    len += glcdCalcStringLen(":");
    len += glcdCalcStringLen(sec);

    glcdSetXY((rect.w - len) / 2, 32);

    drawTm(hour, setup->child == RTC_HOUR);
    glcdWriteString(":");
    drawTm(min, setup->child == RTC_MIN);
    glcdWriteString(":");
    drawTm(sec, setup->child == RTC_SEC);

    glcdSetStringFramed(false);
}


static void drawDate(bool clear)
{
    (void)clear;

    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    RTC_type rtc;
    rtcGetTime(&rtc);

    glcdSetFont(&fontterminus32);

    char date[3], month[3], year[5];
    int16_t len = 0;

    snprintf(date, sizeof(date), "%02d", rtc.date);
    snprintf(month, sizeof(month), "%02d", rtc.month);
    snprintf(year, sizeof(year), "20%02d", rtc.year);

    glcdSetStringFramed(true);

    len += glcdCalcStringLen(date);
    len += glcdCalcStringLen(".");
    len += glcdCalcStringLen(month);
    len += glcdCalcStringLen(".");
    len += glcdCalcStringLen(year);

    glcdSetXY((rect.w - len) / 2, 32);

    drawTm(date, setup->child == RTC_DATE);
    glcdWriteString(".");
    drawTm(month, setup->child == RTC_MONTH);
    glcdWriteString(".");
    drawTm(year, setup->child == RTC_YEAR);

    glcdSetStringFramed(false);
}

static void drawAlarm(bool clear)
{
    Alarm *alarm = rtcGetAlarm(0);

    static AlarmDay _days;
    if (alarm->days != _days) {
        clear = true;
        _days = alarm->days;
    }

    GlcdRect rect = glcdGet()->rect;

    const Palette *pal = paletteGet();
    Setup *setup = setupGet();

    glcdSetFont(&fontterminus32);

    char hour[3], min[3], days[32];
    int16_t len = 0;

    const char *daysLabel = labelsGet((Label)(LABEL_ALARM_DAY_OFF + (alarm->days)));

    snprintf(hour, sizeof(hour), "%02d", alarm->hour);
    snprintf(min, sizeof(min), "%02d", alarm->min);
    snprintf(days, sizeof(days), "%s", daysLabel);

    glcdSetStringFramed(true);

    len += glcdCalcStringLen(hour);
    len += glcdCalcStringLen(":");
    len += glcdCalcStringLen(min);
    len += glcdCalcStringLen(" ");
    len += glcdCalcStringLen(days);

    if (clear) {
        const int16_t marginX = 16;
        glcdDrawRect(marginX, 32, rect.w - marginX * 2, 32, pal->bg);
    }

    glcdSetXY((rect.w - len) / 2, 32);

    drawTm(hour, setup->child == ALARM_HOUR);
    glcdWriteString(":");
    drawTm(min, setup->child == ALARM_MIN);
    glcdWriteString(" ");
    drawTm(days, setup->child == ALARM_DAYS);

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
    case SETUP_ALARM:
        drawAlarm(clear);
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
