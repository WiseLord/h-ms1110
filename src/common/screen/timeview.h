#ifndef TIMEVIEW_H
#define TIMEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

void timeViewDraw(bool clear, bool active, int8_t hour, int8_t min, int8_t sec);
void dateViewDraw(bool clear, bool active, int8_t date, int8_t month, int8_t year, int8_t wday);
void wdayViewDraw(bool clear, bool active, int8_t wday);

#ifdef __cplusplus
}
#endif

#endif // TIMEVIEW_H
