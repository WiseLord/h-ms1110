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

#define AMP_SYNC_DATASIZE   16

typedef uint8_t SyncType;
enum {
    SYNC_NONE = 0,

    SYNC_ACTION,
    SYNC_TIME,
    SYNC_SPECTRUM,
    SYNC_IN_TYPE,

    SYNC_END
};

typedef union {
    uint8_t data[AMP_SYNC_DATASIZE];
    struct {
        SyncType type;
        union {
            uint32_t time;
            Action action;
            Spectrum spectrum;
            uint8_t inType;
        };
    };
} AmpSync;

void syncMasterSendTime(uint8_t slaveAddr, uint32_t time);
void syncMasterSendAction(uint8_t slaveAddr, Action *action);
void syncMasterSendSpectrum(uint8_t slaveAddr, Spectrum *spectrum);
void syncMasterSendInType(uint8_t slaveAddr, uint8_t inType);
void syncMasterReceive(uint8_t slaveAddr, AmpSync *sync);

void syncSlaveInit(uint8_t addr);
void syncSlaveSendAction(Action *action);
void syncSlaveReceive(uint8_t **data, uint8_t *size);

#ifdef __cplusplus
}
#endif

#endif // SYNC_H
