#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "../display/glcd.h"

typedef struct {
    Glcd *glcd;
} Canvas;

void canvasInit(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear);
void canvasShowTime(bool clear);
void canvasShowStandby(bool clear);
void canvasShowTune(bool clear);
void canvasShowAudioInput(bool clear);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
