#ifndef TUNEVIEW_H
#define TUNEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef uint8_t TuneFlags;
enum {
    TUNE_FLAG_INIT      = 0x00,
    TUNE_FLAG_VALUE     = 0x01,
};

typedef struct {
    const char *name;
    char valStr[8];
    char unitStr[12];

    int16_t value;
    int16_t min;
    int16_t max;
    TuneFlags flags;
} TuneView;

void tuneViewDraw(TuneView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // TUNEVIEW_H
