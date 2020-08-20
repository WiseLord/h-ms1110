#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/glcd.h"
#include "gui/palette.h"
#include "screen/radioview.h"
#include "screen/tuneview.h"
#include "spectrum.h"
#include "tr/labels.h"

typedef struct {
    Glcd *glcd;
    const Palette *pal;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks);
void canvasShowTime(bool clear, bool active);
void canvasShowDate(bool clear, bool active);
void canvasShowWday(bool clear, bool active);
void canvasShowInput(bool clear, int8_t input, Label label);
void canvasShowTune(bool clear, TuneView *tune);
void canvasShowSetup(bool clear);
void canvasShowRadio(bool clear, RadioView *radio);

void canvasDebugFPS(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
