#include "datetimeview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"
#include "tr/labels.h"

static const GlcdRect rectTime = {76, 0, 103, 40};
static const GlcdRect rectDateTop = {0, 0, 256, 32};
static const GlcdRect rectDateMiddle = {0, 16, 256, 32};
static const GlcdRect rectDateBottom = {0, 32, 256, 32};

extern const tFont fontterminusdig40;

static DateTimeView dtv;

static void timeDraw(DateTimeView *this, bool clear)
{
    if (this->sec != dtv.sec || this->min != dtv.min || this->hour != dtv.hour) {
        clear = true;
        dtv.hour = this->hour;
        dtv.min = this->min;
        dtv.sec = this->sec;
    }

    if (clear) {
        const tFont *font = &fontterminusdig40;
        const Palette *pal = paletteGet();

        glcdSetFont(font);
        glcdSetFontColor(pal->fg);
        glcdSetFontBgColor(pal->bg);

        char buf[64];
        const char *dlm = "\u2008:\u2008";

        snprintf(buf, sizeof(buf), "%02d%s%02d", this->hour, dlm, this->min);

        glcdSetRect(&rectTime);

        glcdSetXY(0, 0);
        glcdWriteString(buf);

        glcdResetRect();
    }
}

static void dateDraw(DateTimeView *this, bool clear, DateTimeMode mode, const GlcdRect *rect)
{
    if (this->date != dtv.date || this->month != dtv.month || this->year != dtv.year) {
        clear = true;
        dtv.date = this->date;
        dtv.month = this->month;
        dtv.year = this->year;
        dtv.wday = this->wday;
    }

    if (clear) {
        const tFont *font = &fontterminus28b;
        const Palette *pal = paletteGet();

        glcdSetFont(font);
        glcdSetFontColor(pal->fg);
        glcdSetFontBgColor(pal->bg);

        char buf[64];

        if (mode == DT_MODE_DATE) {
            const char *monthLabel = labelsGet((Label)(LABEL_JANUARY + dtv.month - 1));
            snprintf(buf, sizeof(buf), "%d %s 20%02d", dtv.date, monthLabel, dtv.year);
        } else if (mode == DT_MODE_WDAY) {
            const char *wdayLabel = labelsGet((Label)(LABEL_SUNDAY + dtv.wday));
            snprintf(buf, sizeof(buf), "%s", wdayLabel);
        }

        glcdSetRect(rect);
        glcdSetXY(rect->w / 2, 0);
        glcdSetFontAlign(GLCD_ALIGN_CENTER);
        int16_t len = glcdWriteString(buf);

        int16_t left = (rect->w - len + 1) / 2;

        // Clear everything else
        glcdDrawRect(0, 0, left, 32, pal->bg);
        glcdDrawRect(left + len, 0, rect->w - left - len, 32, pal->bg);

        glcdResetRect();
    }
}

void dateTimeViewDraw(DateTimeView *this, bool clear)
{
    if (this->mode & DT_MODE_TIME) {
        timeDraw(this, clear);
    } else {
        if ((this->mode & (DT_MODE_DATE | DT_MODE_WDAY)) == (DT_MODE_DATE | DT_MODE_WDAY)) {
            dateDraw(this, clear, DT_MODE_DATE, &rectDateTop);
            dateDraw(this, clear, DT_MODE_WDAY, &rectDateBottom);
        } else if (this->mode & DT_MODE_DATE) {
            dateDraw(this, clear, DT_MODE_DATE, &rectDateMiddle);
        } else if (this->mode & DT_MODE_WDAY) {
            dateDraw(this, clear, DT_MODE_WDAY, &rectDateMiddle);
        }
    }
}
