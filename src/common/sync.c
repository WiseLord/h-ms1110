#include "sync.h"

#include <stddef.h>
#include <string.h>

#include "amp.h"
#include "hwlibs.h"
#include "i2c.h"

uint8_t ampSyncRxData[AMP_SYNC_DATASIZE];
uint8_t ampSyncRxSize;

uint8_t ampSyncTxData[AMP_SYNC_DATASIZE];
uint8_t ampSyncTxSize;

static void ampSyncRxCb(int16_t bytes)
{
    if (ampSyncRxData[0] != SYNC_NONE) {
        if (bytes > AMP_SYNC_DATASIZE) {
            bytes = 0;
        }
        ampSyncRxSize = bytes;
    }
}

static void ampSyncTxCb(int16_t bytes)
{
    SyncType type = ampSyncTxData[0];
    ampSyncTxData[0] = SYNC_NONE;

    i2cBegin(I2C_SYNC, 0x00);
    i2cSend(I2C_SYNC, type);
    switch (type) {
    case SYNC_ACTION:
        for (size_t i = 0; i < sizeof(Action); i++) {
            i2cSend(I2C_SYNC, ampSyncTxData[1 + i]);
        }
        break;
    }
}

static void syncMasterSend(uint8_t slaveAddr, SyncType type, uint8_t *data, size_t size)
{
    i2cBegin(I2C_SYNC, slaveAddr);
    i2cSend(I2C_SYNC, type);
    for (size_t i = 0; i < size; i++) {
        i2cSend(I2C_SYNC, data[i]);
    }
    i2cTransmit(I2C_SYNC);
}

void syncMasterSendTime(uint8_t slaveAddr, uint32_t time)
{
    SyncType type = SYNC_TIME;
    uint8_t *data = (uint8_t *)&time;
    size_t size = sizeof(uint32_t);

    syncMasterSend(slaveAddr, type, data, size);
}

void syncMasterSendAction(uint8_t slaveAddr, Action *action)
{
    SyncType type = SYNC_ACTION;
    uint8_t *data = (uint8_t *)action;
    size_t size = sizeof(Action);

    syncMasterSend(slaveAddr, type, data, size);
}

void syncMasterSendSpectrum(uint8_t slaveAddr, Spectrum *spectrum)
{
    SyncType type = SYNC_SPECTRUM;
    uint8_t *data = (uint8_t *)spectrum;
    size_t size = sizeof(Spectrum);

    syncMasterSend(slaveAddr, type, data, size);
}

void syncMasterSendInType(uint8_t slaveAddr, uint8_t inType)
{
    SyncType type = SYNC_IN_TYPE;
    uint8_t *data = (uint8_t *)&inType;
    size_t size = sizeof(uint8_t);

    syncMasterSend(slaveAddr, type, data, size);
}

SyncType syncMasterReceive(uint8_t slaveAddr, uint8_t *data)
{
    SyncType type = SYNC_NONE;

    i2cBegin(I2C_SYNC, slaveAddr);
    i2cReceive(I2C_SYNC, &type, 1);

    switch (type) {
    case SYNC_ACTION:
        i2cBegin(I2C_SYNC, slaveAddr);
        i2cReceive(I2C_SYNC, data, 1 + sizeof(Action));
        break;
    }

    return type;
}

void syncSlaveInit(uint8_t addr)
{
    i2cInit(I2C_SYNC, 400000);
    i2cSetRxCb(I2C_SYNC, ampSyncRxCb);
    i2cSetTxCb(I2C_SYNC, ampSyncTxCb);
    i2cBegin(I2C_SYNC, addr);
    i2cSlaveTransmitReceive(I2C_SYNC, ampSyncRxData, sizeof(ampSyncRxData));
}

void syncSlaveSendAction(Action *action)
{
    ampSyncTxData[0] = SYNC_ACTION;
    memcpy(&ampSyncTxData[1], action, sizeof(Action));
}

void syncSlaveReceive(uint8_t **data, uint8_t *size)
{
    *data = ampSyncRxData;
    *size = ampSyncRxSize;

    ampSyncRxSize = 0;
}
