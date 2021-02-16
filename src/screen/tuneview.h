#ifndef TUNEVIEW_H
#define TUNEVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
    Label label;
} TuneView;

void tuneViewDraw(bool clear, TuneView *tune);

#ifdef __cplusplus
}
#endif

#endif // TUNEVIEW_H
