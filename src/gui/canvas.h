#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "amp.h"
#include "audio/audiodefs.h"
#include "display/glcd.h"
#include "gui/palette.h"
#include "spectrum.h"
#include "tr/labels.h"

#include "view/datetimeview.h"
#include "view/tuneview.h"

typedef struct {
    Glcd *glcd;
    const Palette *pal;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks);
void canvasShowStars(bool clear, int16_t offset);

void canvasShowDateTime(bool clear, DateTimeMode mode);
void canvasShowInputCommon(bool clear, InputType inType);

void canvasShowInputSpectrum(bool clear);
void canvasShowInputTuner(bool clear);
void canvasShowInputMpc(bool clear);
void canvasShowInputSelector(bool clear, int8_t inIdx, bool inIdxUp, const void *inMap);
void canvasShowInput(bool clear, InputType inType);
void canvasShowMute(bool clear);

void canvasShowTune(bool clear, AudioTune tune);
void canvasShowSetup(bool clear);

void canvasDebugFPS(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
