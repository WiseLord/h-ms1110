#include "tunersync.h"

#include <string.h>

#include "hwlibs.h"
#include "sync.h"
#include "tunersync.h"

static TunerSync tunerSync;

void tunerSyncInit(void)
{
    tunerSync.rdsParser = rdsParserGet();
}

void tunerSyncRequest(void)
{
    syncMasterSend(AMP_TUNER_ADDR, SYNC_REQUEST, NULL, 0);
}

TunerSync *tunerSyncGet()
{
    return &tunerSync;
}
