#ifndef SPVIEW_H
#define SPVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "spectrum.h"

void spViewDraw(bool clear, bool check, bool mirror, bool peaks, SpChan chan, GlcdRect *rect);

#ifdef __cplusplus
}
#endif

#endif // SPVIEW_H
