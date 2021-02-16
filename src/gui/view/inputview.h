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
} InputView;

void inputViewDraw(InputView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // INPUTVIEW_H
