#ifndef TUNE_H
#define TUNE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "fonts.h"
#include "progressbar.h"
#include "tr/labels.h"

typedef const struct {
    struct {
        const tFont *font;
    } value;
    struct {
        const tFont *font;
    } label;
    struct {
        int16_t y;
    } bar;
} LayoutTune;

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
    Label label;
} Tune;

void tuneDraw(bool clear, Tune *pview, LayoutTune *lt);

#ifdef __cplusplus
}
#endif

#endif // TUNE_H
