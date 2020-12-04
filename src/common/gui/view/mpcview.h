#ifndef MPCVIEW_H
#define MPCVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef struct {
    char *meta;
    int16_t elapsed;
    int16_t duration;
    int16_t scroll_oft;
    bool scroll_event;
    bool scroll_left;
    int16_t scroll_pause;
} MpcView;

void mpcViewDraw(MpcView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // MPCVIEW_H
