#include "sync.h"

#include <stddef.h>
#include <string.h>

#include "hwlibs.h"
#include "i2c.h"

#include "action.h"
#include "spectrum.h"
#include "tunersync.h"

uint8_t ampSyncRxData[SYNC_DATASIZE];
uint8_t ampSyncRxSize;

uint8_t ampSyncTxData[SYNC_DATASIZE];
uint8_t ampSyncTxSize;
volatile bool ampSyncTxIsBusy;

static void ampSyncRxCb(int16_t bytes)
{
    if (ampSyncRxData[0] != SYNC_NONE) {
        if (bytes > SYNC_DATASIZE) {
            bytes = 0;
        }
        ampSyncRxSize = bytes;
    }
}

static void ampSyncTxCb(int16_t bytes)
{
    (void)bytes;

    i2cBegin(I2C_SYNC, 0x00);
    if (ampSyncTxSize > 0) {
        for (size_t i = 0; i < ampSyncTxSize; i++) {
            i2cSend(I2C_SYNC, ampSyncTxData[i]);
        }
        ampSyncTxSize = 0;
    } else {
        i2cSend(I2C_SYNC, SYNC_NONE);
    }

    ampSyncTxIsBusy = false;
}

SyncType syncMasterSend(uint8_t slaveAddr, SyncType type, void *data, size_t size)
{
    i2cBegin(I2C_SYNC, slaveAddr);
    i2cSend(I2C_SYNC, type);
    for (size_t i = 0; i < size; i++) {
        i2cSend(I2C_SYNC, ((uint8_t *)data)[i]);
    }

    bool i2cRet = i2cTransmit(I2C_SYNC);

    if (!i2cRet) {
        return SYNC_ERR;
    }

    return SYNC_NONE;
}

void syncMasterInit(void)
{
    i2cInit(I2C_SYNC, 400000, 0x00);
}

SyncType syncMasterReceive(uint8_t slaveAddr, uint8_t *data)
{
    SyncType type = SYNC_NONE;

    i2cBegin(I2C_SYNC, slaveAddr);

    bool i2cRet = i2cReceive(I2C_SYNC, &type, 1);

    if (!i2cRet) {
        return SYNC_ERR;
    }

    switch (type) {
    case SYNC_ACTION:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(Action));
        break;
    case SYNC_SPECTRUM:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(Spectrum));
        break;
    case SYNC_TUNER_FREQ:
    case SYNC_TUNER_FAVS:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(uint16_t));
        break;
    case SYNC_TUNER_STNUM:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(int8_t));
        break;
    case SYNC_TUNER_FLAGS:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(TunerFlag));
        break;
    case SYNC_TUNER_BAND:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(TunerSyncBand));
        break;
    case SYNC_TUNER_RDS:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(RdsParser));
        break;
    }

    return type;
}

void syncSlaveInit(uint8_t addr)
{
    i2cInit(I2C_SYNC, 400000, addr);
    i2cSetRxCb(I2C_SYNC, ampSyncRxCb);
    i2cSetTxCb(I2C_SYNC, ampSyncTxCb);
    i2cBegin(I2C_SYNC, addr);
    i2cSlaveTransmitReceive(I2C_SYNC, ampSyncRxData, sizeof(ampSyncRxData));
}

void syncSlaveSend(SyncType type, void *data, size_t size)
{
    if (ampSyncTxIsBusy) {
        return;
    }

    ampSyncTxIsBusy = true;

    ampSyncTxData[0] = type;
    if (size > 0) {
        memcpy(&ampSyncTxData[1], data, size);
    }
    ampSyncTxSize = size + 1;
}

void syncSlaveReceive(uint8_t **data, uint8_t *size)
{
    *data = ampSyncRxData;
    *size = ampSyncRxSize;

    ampSyncRxSize = 0;
}

bool syncTxIsBusy(void)
{
    return ampSyncTxIsBusy;
}

void syncTxResetBusy()
{
    ampSyncTxIsBusy = false;
}
