#include "sync.h"

#include <stddef.h>
#include <string.h>

#include "amp.h"
#include "hwlibs.h"
#include "i2c.h"

static AmpSync ampSyncTx;

uint8_t ampSyncRxData[AMP_SYNC_DATASIZE];
uint8_t ampSyncRxSize;

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
    if (ampSyncTx.type != SYNC_NONE) {
        i2cBegin(I2C_SYNC, 0x28);
        for (size_t i = 0; i < sizeof(ampSyncTx); i++) {
            i2cSend(I2C_SYNC, ampSyncTx.data[i]);
        }
    } else {
        i2cBegin(I2C_SYNC, 0x28);
        for (size_t i = 0; i < sizeof (ampSyncTx); i++) {
            i2cSend(I2C_SYNC, 0x00);
        }
    }
    memset(&ampSyncTx, 0x00, sizeof(ampSyncTx));
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

void syncMasterReceive(uint8_t slaveAddr, AmpSync *sync)
{
    i2cBegin(I2C_SYNC, slaveAddr);
    i2cReceive(I2C_SYNC, sync->data, sizeof (AmpSync));
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
    ampSyncTx.type = SYNC_ACTION;
    ampSyncTx.action = *action;
}

void syncSlaveReceive(uint8_t **data, uint8_t *size)
{
    *data = ampSyncRxData;
    *size = ampSyncRxSize;

    ampSyncRxSize = 0;
}
