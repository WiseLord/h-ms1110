#ifndef SYNC_H
#define SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define SYNC_DATASIZE   16
#define SYNC_I2C_FREQ   400000

typedef uint8_t SyncType;
enum {
    SYNC_NONE = 0,

    SYNC_ACTION,
    SYNC_TIME,
    SYNC_SPECTRUM,
    SYNC_IN_TYPE,

    SYNC_TUNER_BAND,
    SYNC_TUNER_FREQ,
    SYNC_TUNER_FLAGS,

    SYNC_END
};

void syncMasterInit(void);
void syncMasterSend(uint8_t slaveAddr, SyncType type, void *data, size_t size);
SyncType syncMasterReceive(uint8_t slaveAddr, uint8_t *data);

void syncSlaveInit(uint8_t addr);
void syncSlaveSend(SyncType type, void *data, size_t size);
void syncSlaveReceive(uint8_t **data, uint8_t *size);

#ifdef __cplusplus
}
#endif

#endif // SYNC_H
