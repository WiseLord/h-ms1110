#ifndef SETUPVIEW_H
#define SETUPVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "setup.h"

typedef struct {
    Setup *setup;
} SetupView;

void setupViewDraw(SetupView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // SETUPVIEW_H
