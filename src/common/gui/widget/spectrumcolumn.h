#ifndef SPECTRUMCOLUMN_H
#define SPECTRUMCOLUMN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/glcd.h"

typedef struct {
    int16_t showW;
    int16_t prevW;
    int16_t peakW;
    int16_t fallW;
} SpectrumColumn;

void spectrumColumnDraw(SpectrumColumn *this, bool clear, GlcdRect *rect, bool mirror, color_t *grad);

#ifdef __cplusplus
}
#endif

#endif // SPECTRUMCOLUMN_H
