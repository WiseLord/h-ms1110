#ifndef PARAMVIEW_H
#define PARAMVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "display/fonts/fonts.h"

typedef const struct {
    const tFont *valFont;
    int16_t valX;
    int16_t valY;
} LayoutParamView;

typedef struct {
    int16_t val;
} ParamView;

void paramViewDraw(bool clear, ParamView *pview, LayoutParamView *lt);

#ifdef __cplusplus
}
#endif

#endif // PARAMVIEW_H
