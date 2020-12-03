#ifndef MPCVIEW_H
#define MPCVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef struct {
    char *name;
} MpcView;

void mpcViewDraw(bool clear, MpcView *view);

#ifdef __cplusplus
}
#endif

#endif // MPCVIEW_H
