#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "amp.h"
#include "display/glcd.h"
#include "gui/palette.h"
#include "screen/tuneview.h"
#include "spectrum.h"
#include "tr/labels.h"

#include "view/datetimeview.h"

typedef struct {
    Glcd *glcd;
    const Palette *pal;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks);
void canvasShowDemo(bool clear, int16_t offset);

void canvasShowDateTime(bool clear, DateTimeMode mode);

void canvasShowInputCommon(InputType inType, bool clear);
void canvasShowInputTuner(bool clear);
void canvasShowInputMpc(bool clear);
void canvasShowInputDefault(bool clear);
void canvasShowInput(InputType inType, bool clear);

void canvasShowTune(bool clear, TuneView *tune);
void canvasShowSetup(bool clear);

void canvasDebugFPS(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
