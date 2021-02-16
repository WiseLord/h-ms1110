#ifndef MPCVIEW_H
#define MPCVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "gui/widget/scrolltext.h"
#include "mpc.h"

typedef struct {
    Mpc *mpc;
    ScrollText scroll;
} MpcView;

void mpcViewDraw(MpcView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // MPCVIEW_H
