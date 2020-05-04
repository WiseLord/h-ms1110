#include "sync.h"

#include <stddef.h>
#include <string.h>

#include "amp.h"
#include "hwlibs.h"
#include "i2c.h"

static AmpSync ampSync;

static AmpSync ampSyncRx;
static AmpSync ampSyncTx;


static void ampSyncRxCb(int16_t bytes)
{
    (void)bytes;

    if (ampSyncRx.type != SYNC_NONE) {
        ampSync = ampSyncRx;
        ampSyncRx.type = SYNC_NONE;
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

void syncMasterSendTime(uint8_t slaveAddr, uint32_t time)
{
    AmpSync sync;

    sync.type = SYNC_TIME;
    sync.time = time;

    i2cBegin(I2C_SYNC, slaveAddr);
    for (size_t i = 0; i < sizeof(sync); i++) {
        i2cSend(I2C_SYNC, sync.data[i]);
    }
    i2cTransmit(I2C_SYNC);
}

void syncMasterSendAction(uint8_t slaveAddr, Action *action)
{
    AmpSync sync;

    sync.type = SYNC_ACTION;
    sync.action = *action;

    i2cBegin(I2C_SYNC, slaveAddr);
    for (size_t i = 0; i < sizeof(sync); i++) {
        i2cSend(I2C_SYNC, sync.data[i]);
    }
    i2cTransmit(I2C_SYNC);
}

void syncMasterSendSpectrum(uint8_t slaveAddr, Spectrum *spectrum)
{
    AmpSync sync;

    sync.type = SYNC_SPECTRUM;
    sync.spectrum = *spectrum;

    i2cBegin(I2C_SYNC, slaveAddr);
    for (size_t i = 0; i < sizeof(sync); i++) {
        i2cSend(I2C_SYNC, sync.data[i]);
    }
    i2cTransmit(I2C_SYNC);
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
    i2cSlaveTransmitReceive(I2C_SYNC, ampSyncRx.data, sizeof(ampSyncRx));
}

void syncSlaveSendAction(Action *action)
{
    ampSyncTx.type = SYNC_ACTION;
    ampSyncTx.action = *action;
}

void syncSlaveReceive(AmpSync *sync)
{
    *sync = ampSync;
    ampSync.type = SYNC_NONE;
}
