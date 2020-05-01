#ifndef TUNE_H
#define TUNE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "tr/labels.h"

typedef struct {
    int16_t value;
    int16_t min;
    int16_t max;
    Label label;
} Tune;

void tuneDraw(bool clear, Tune *pview);

#ifdef __cplusplus
}
#endif

#endif // TUNE_H
