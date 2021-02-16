#ifndef PALETTE_H
#define PALETTE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "display/colors.h"

typedef struct {
    color_t fg;     // Foreground
    color_t bg;     // Background

    color_t active;
    color_t inactive;

    color_t spColB;
    color_t spColG;
    color_t spPeak;
} Palette;

typedef int8_t PalIdx;
enum {
    PAL_DEFAULT = 0,

    PAL_END
};

const Palette *paletteGet(void);
PalIdx paletteGetIndex(void);
void paletteSetIndex(PalIdx index);

#ifdef __cplusplus
}
#endif

#endif // PALETTE_H
