#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "spectrum.h"

void spViewDraw(bool clear, bool check, bool mirror, bool peaks, SpChan chan, GlcdRect *rect);

void *spViewGetDrawData(void);

#ifdef __cplusplus
}
#endif

#endif // SPECTRUMVIEW_H
