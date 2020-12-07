#ifndef MPCVIEW_H
#define MPCVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "mpc.h"

typedef struct {
    Mpc *mpc;
    struct {
        int16_t oft;
        int16_t pause;
        bool event;
        bool left;
    } scroll;
} MpcView;

void mpcViewDraw(MpcView *this, bool clear);

#ifdef __cplusplus
}
#endif

#endif // MPCVIEW_H
