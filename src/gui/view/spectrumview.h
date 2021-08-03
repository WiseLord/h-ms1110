#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "spectrum.h"

typedef struct {
    bool check;
    bool mirror;
    bool peaks;
    SpChan chan;
} SpView;

void spViewDraw(SpView *this, bool clear, GlcdRect *rect);

void *spViewGetDrawData(void);

#ifdef __cplusplus
}
#endif

#endif // SPECTRUMVIEW_H
