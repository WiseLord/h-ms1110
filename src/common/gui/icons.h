#ifndef ICONS_H
#define ICONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "fonts.h"

typedef uint8_t Icon;
enum {
    ICON_EMPTY = 0x00,

    // Player status icons
    ICON_STOPPED = 0x10,
    ICON_PLAYING,
    ICON_PAUSED,

    // Input icons
    ICON_TUNER = 0x40,
    ICON_MPD,
    ICON_AUX1,
    ICON_AUX2,
};

extern const tFont icons_hms1110;

const tImage *iconFind(Icon code, const tFont *iFont);

#ifdef __cplusplus
}
#endif

#endif // ICONS_H
