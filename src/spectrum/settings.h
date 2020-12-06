#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "action.h"
#include "eemul.h"

typedef uint8_t Param;
enum {
    PARAM_NULL = 0,

    PARAM_SPECTRUM_MODE,
    PARAM_SPECTRUM_PEAKS,
    PARAM_SPECTRUM_GRAD,

    PARAM_SYSTEM_LANG,
    PARAM_SYSTEM_ENC_RES,
    PARAM_SYSTEM_RTC_CORR,

    PARAM_END
};

void settingsInit(void);

int16_t settingsGet(Param param);
void settingsSet(Param param, int16_t value);

int16_t settingsRead(Param param);
void settingsStore(Param param, int16_t value);

const EE_Cell *eeMapGet(void);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
