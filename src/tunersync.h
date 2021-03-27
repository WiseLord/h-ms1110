#ifndef TUNERSYNC_H
#define TUNERSYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "tuner/rds/parser.h"
#include "tuner/stations.h"
#include "tuner/tuner.h"

typedef uint8_t TunerSyncFlags;
enum {
    TUNERSYNC_FLAG_NO_FLAG = 0x00,

    TUNERSYNC_FLAG_FREQ    = 0x01,
    TUNERSYNC_FLAG_FAVS    = 0x02,
    TUNERSYNC_FLAG_BAND    = 0x04,
    TUNERSYNC_FLAG_STNUM   = 0x08,

    TUNERSYNC_FLAG_STATUS  = 0x10,
    TUNERSYNC_FLAG_PARAM   = 0x20,
    TUNERSYNC_FLAG_RDS     = 0x40,
};

typedef struct {
    uint16_t fMin;
    uint16_t fMax;
} TunerSyncBand;

typedef struct {
    uint16_t freq;
    uint16_t favMask;
    TunerSyncBand band;
    int8_t stNum;
    TunerSyncFlags flags;
    TunerStatusFlag statusFlags;
    TunerParamFlag paramFlags;
} TunerSync;

void tunerSyncInit(void);
TunerSync *tunerSyncGet(void);

#ifdef __cplusplus
}
#endif

#endif // TUNERSYNC_H
