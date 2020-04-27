#ifndef CANVAS_H
#define CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/glcd.h"
#include "gui/layout.h"
#include "gui/palette.h"

#include "spectrum.h"

typedef uint8_t ScreenType;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,
    SCREEN_INPUT,

    SCREEN_STANDBY,

    SCREEN_TUNE,

    SCREEN_END
};

typedef struct {
    Glcd *glcd;
    const Palette *pal;
    const Layout *layout;
} Canvas;

void canvasInit(void);
Canvas *canvasGet(void);

void canvasClear(void);

void canvasShowSpectrum(bool clear, SpMode mode, bool peaks);

void canvasShowInput(bool clear, Label label);
void canvasShowTune(bool clear, Tune *tune);

void canvasShowTest(bool clear);
void canvasDebugFPS(void);

#ifdef __cplusplus
}
#endif

#endif // CANVAS_H
