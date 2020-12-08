#ifndef TUNERVIEW_H
#define TUNERVIEW_H

#include <stdbool.h>
#include <stdint.h>

#include "tunersync.h"

typedef struct {
    TunerSync sync;
} TunerView;

void tunerViewDraw(TunerView *this, bool clear);

#endif // TUNERVIEW_H
