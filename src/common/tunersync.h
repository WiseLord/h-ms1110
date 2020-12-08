#ifndef TUNERSYNC_H
#define TUNERSYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "tuner/tuner.h"

typedef uint16_t TunerSyncFlags;
enum {
    TUNERSYNC_FLAG_FREQ    = 0x0001,
    TUNERSYNC_FLAG_FLAGS   = 0x0002,
    TUNERSYNC_FLAG_FAVS    = 0x0004,
    TUNERSYNC_FLAG_BAND    = 0x0008,
};

typedef struct {
    uint16_t fMin;
    uint16_t fMax;
} TunerSyncBand;

typedef struct {
    TunerSyncFlags flags;
    uint16_t freq;
    TunerFlag tFlags;
    uint16_t favMask;
    TunerSyncBand band;
} TunerSync;

void tunerSyncInit(void);
TunerSync *tunerSyncGet(void);

#ifdef __cplusplus
}
#endif

#endif // TUNERSYNC_H
