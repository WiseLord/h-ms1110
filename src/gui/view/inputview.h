#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "gui/icons.h"

typedef struct {
    const char *name;
    const void *inMap;
    int16_t scrollTimer;
    int8_t inIdx;
} InputView;

void inputViewDraw(InputView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // INPUTVIEW_H
