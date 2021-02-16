#ifndef ICONIMAGE_H
#define ICONIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "gui/icons.h"

typedef struct {
    const GlcdRect *rect;
    color_t color;
    Icon icon;
} IconImage;

void iconImageDraw(IconImage *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // ICONIMAGE_H
