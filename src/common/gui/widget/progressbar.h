#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/glcd.h"

typedef struct {
    GlcdRect rect;
    int16_t fw;
    int16_t sc;
    int8_t sw;
} LayoutProgressBar;

typedef struct {
    LayoutProgressBar lt;
    int16_t value;
    int16_t min;
    int16_t max;
} ProgressBar;

void progressBarDraw(bool clear, ProgressBar *bar);

#ifdef __cplusplus
}
#endif

#endif // PROGRESSBAR_H
