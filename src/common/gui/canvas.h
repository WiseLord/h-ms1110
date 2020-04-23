#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "gui/layout.h"
#include "gui/palette.h"

#include "spectrum.h"

typedef struct {
    Glcd *glcd;
    const Palette *pal;
    const Layout *layout;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks);

void canvasDebugFPS(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
