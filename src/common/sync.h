#ifndef SYNC_H
#define SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "action.h"
#include "spectrum.h"

#define AMP_TUNER_ADDR      0x28
#define AMP_PLAYER_ADDR     0x29

typedef uint8_t SyncType;
enum {
    SYNC_NONE = 0,

    SYNC_ACTION,
    SYNC_TIME,
    SYNC_SPECTRUM,

    SYNC_END
};

typedef union {
    uint8_t data[8];
    struct {
        SyncType type;
        union {
            Action action;
            uint32_t time;
            Spectrum spectrum;
        };
    };
} AmpSync;

void syncMasterSendTime(uint8_t slaveAddr, uint32_t time);
void syncMasterSendAction(uint8_t slaveAddr, Action *action);
void syncMasterSendSpectrum(uint8_t slaveAddr, Spectrum *spectrum);
void syncMasterReceive(uint8_t slaveAddr, AmpSync *sync);

void syncSlaveInit(uint8_t addr);
void syncSlaveSendAction(Action *action);
void syncSlaveReceive(AmpSync *sync);

#ifdef __cplusplus
}
#endif

#endif // SYNC_H
