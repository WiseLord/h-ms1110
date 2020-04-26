#ifndef LAYOUT_H
#define LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "display/fonts/fonts.h"

#include "gui/widget/tune.h"

typedef struct {
    LayoutTune par;
} Layout;

const Layout *layoutGet(void);

extern const tFont fontterminusdig40;
extern const tFont fontterminusdig64;

#ifdef __cplusplus
}
#endif

#endif // LAYOUT_H
