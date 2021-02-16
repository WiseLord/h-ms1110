#include "tunersync.h"

#include <string.h>

#include "hwlibs.h"
#include "sync.h"
#include "tunersync.h"

static TunerSync tunerSync;

void tunerSyncInit(void)
{
    memset(&tunerSync, 0, sizeof(TunerSync));
}

TunerSync *tunerSyncGet()
{
    return &tunerSync;
}
