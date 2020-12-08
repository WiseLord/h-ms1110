#include "tunersync.h"

static TunerSync tunerSync;

TunerSync *tunerSyncGet()
{
    return &tunerSync;
}
