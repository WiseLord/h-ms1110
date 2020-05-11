#ifndef RADIOVIEW_H
#define RADIOVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef struct {
    uint16_t freq;
    bool stereo;
} RadioView;

void radioViewDraw(bool clear, RadioView *radio);

#ifdef __cplusplus
}
#endif

#endif // RADIOVIEW_H
