#ifndef LAYOUT_H
#define LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "display/glcd.h"

typedef struct {
    const tFont *lblFont;       // Main label font
} Layout;

const Layout *layoutGet(void);

extern const tFont fontterminusdig40;
extern const tFont fontterminusdig64;

#ifdef __cplusplus
}
#endif

#endif // LAYOUT_H
