#ifndef DATETIMEVIEW_H
#define DATETIMEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "gui/icons.h"

typedef uint8_t DateTimeMode;
enum {
    DT_MODE_TIME    = 0x01,
    DT_MODE_DATE    = 0x02,
    DT_MODE_WDAY    = 0x04,
};

typedef struct {
    int8_t hour;
    int8_t min;
    int8_t sec;
    int8_t date;
    int8_t month;
    int8_t year;
    int8_t wday;
    DateTimeMode mode;
} DateTimeView;

void dateTimeViewDraw(DateTimeView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // DATETIMEVIEW_H
