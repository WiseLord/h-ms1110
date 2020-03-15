#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "gui/layout.h"
#include "gui/palette.h"

typedef struct {
    Glcd *glcd;
    const Palette *pal;
    const Layout *layout;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear);
void canvasShowTime(bool clear);
void canvasShowStandby(bool clear);
void canvasShowTune(bool clear);
void canvasShowAudioInput(bool clear);
void canvasShowMenu(bool clear);

void canvasShowTest1(bool clear);
void canvasShowTest2(bool clear);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H