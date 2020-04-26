#ifndef TUNE_H
#define TUNE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/fonts/fonts.h"

typedef const struct {
    const tFont *valFont;
    int16_t valX;
    int16_t valY;
} LayoutTune;

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
} Tune;

void tuneDraw(bool clear, Tune *pview, LayoutTune *lt);

#ifdef __cplusplus
}
#endif

#endif // TUNE_H
