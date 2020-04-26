#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "action.h"

#define EE_EMPTY        (uint16_t)0xFFFF
#define EE_NOT_FOUND    (uint16_t)0x8000

#define GENERATE_PARAM_RC(CMD)  PARAM_RC_ ## CMD,

typedef struct {
    uint16_t cell;  // EE cell
    int16_t def;    // Default value
} EE_Map;

typedef uint8_t Param;
enum {
    PARAM_NULL = 0,

    //----------------------------------------------------------------

    PARAM_TUNER_BEGIN,

    PARAM_TUNER_IC = PARAM_TUNER_BEGIN,
    PARAM_TUNER_BAND,
    PARAM_TUNER_STEP,
    PARAM_TUNER_DEEMPH,
    PARAM_TUNER_STA_MODE,
    PARAM_TUNER_FMONO,
    PARAM_TUNER_RDS,
    PARAM_TUNER_BASS,
    PARAM_TUNER_VOLUME,
    PARAM_TUNER_FREQ,

    PARAM_TUNER_END,

    //----------------------------------------------------------------

    PARAM_DISPLAY_DEF = PARAM_TUNER_END,

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

const EE_Map *eeMapGet(void);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
