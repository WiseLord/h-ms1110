#ifndef DRAWSP_H
#define DRAWSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "spectrum.h"

void drawSp(bool clear, bool check, bool mirror, bool peaks, SpChan chan, GlcdRect *rect);

#ifdef __cplusplus
}
#endif

#endif // DRAWSP_H
