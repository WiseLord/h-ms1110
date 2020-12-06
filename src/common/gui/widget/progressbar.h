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
    uint8_t frame_width;
    uint8_t mark_width;
    uint8_t mark_count;
} LayoutProgressBar;

typedef struct {
    LayoutProgressBar lt;
    int16_t value;
    int16_t min;
    int16_t max;
    color_t bgColor;
} ProgressBar;

void progressBarDraw(bool clear, ProgressBar *bar);

#ifdef __cplusplus
}
#endif

#endif // PROGRESSBAR_H
