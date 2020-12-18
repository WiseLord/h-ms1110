#ifndef ICONIMAGE_H
#define ICONIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "display/glcd.h"
#include "gui/icons.h"

void iconImageDraw(bool clear, Icon icon, const GlcdRect *rect, color_t color);

#ifdef __cplusplus
}
#endif

#endif // ICONIMAGE_H
