#include "timeview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"
#include "tr/labels.h"

extern const tFont fontterminusdig40;

void timeViewDraw(bool clear, bool active, int8_t hour, int8_t min, int8_t sec)
{
    static int32_t _hour, _min, _sec;
    if (hour != _hour || min != _min || sec != _sec) {
        clear = true;
        _hour = hour;
        _min = min;
        _sec = sec;
    }

    if (clear) {
        GlcdRect rect = glcdGet()->rect;
        const Palette *pal = paletteGet();
        const tFont *font = &fontterminusdig40;

        glcdSetFont(font);
        glcdSetFontColor(active ? pal->active : pal->inactive);

        char buf[64];
        int16_t sLen;

        snprintf(buf, sizeof(buf), "%02d\u2008:\u2008%02d\u2008:\u2008%02d", hour, min, sec);
        sLen = glcdCalcStringLen(buf);

        glcdSetXY((rect.w - sLen) / 2, 0);
        glcdWriteString(buf);
    }
}

void dateViewDraw(bool clear, bool active, int8_t date, int8_t month, int8_t year, int8_t wday)
{

    static int32_t _date, _month, _year;
    if (date != _date || month != _month || year != _year) {
        clear = true;
        _date = date;
        _month = month;
        _year = year;
    }

    if (clear) {
        GlcdRect rect = glcdGet()->rect;
        const Palette *pal = paletteGet();
        const tFont *font = &fontterminus32;

        glcdDrawRect(rect.x, rect.y, rect.w, rect.h, pal->bg);

        glcdSetFont(font);
        glcdSetFontColor(active ? pal->active : pal->inactive);

        const char *monthLabel = labelsGet((Label)(LABEL_JANUARY + month - 1));

        char buf[64];
        int16_t sLen;

        snprintf(buf, sizeof(buf), "%02d %s 20%02d", date, monthLabel, year);
        sLen = glcdCalcStringLen(buf);

        glcdSetXY((rect.w - sLen) / 2, 0);
        glcdWriteString(buf);

        const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + wday));
        sLen = glcdCalcStringLen(wdayLabel);

        glcdSetXY((rect.w - sLen) / 2, 32);
        glcdWriteString(wdayLabel);
    }
}

void wdayViewDraw(bool clear, bool active, int8_t wday)
{

    static int32_t _wday;
    if (wday != _wday) {
        clear = true;
        _wday = wday;
    }

    if (clear) {
        GlcdRect rect = glcdGet()->rect;
        const Palette *pal = paletteGet();
        const tFont *font = &fontterminus32;

        glcdDrawRect(rect.x, rect.y, rect.w, rect.h, pal->bg);

        glcdSetFont(font);
        glcdSetFontColor(active ? pal->active : pal->inactive);

        int16_t sLen;

        const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + wday));
        sLen = glcdCalcStringLen(wdayLabel);

        glcdSetXY((rect.w - sLen) / 2, 28);
        glcdWriteString(wdayLabel);
    }
}
