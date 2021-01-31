#include "settings.h"

#include "eemul.h"
#include "rc.h"

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = {0x80 + RC_CMD_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Cell eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_AUDIO_IC]        =   {0x01,  0},
    [PARAM_AUDIO_INPUT]     =   {0x02,  0},
    [PARAM_AUDIO_LOUDNESS]  =   {0x03,  false},
    [PARAM_AUDIO_SURROUND]  =   {0x04,  false},
    [PARAM_AUDIO_EFFECT3D]  =   {0x05,  false},
    [PARAM_AUDIO_BYPASS]    =   {0x06,  false},
    [PARAM_AUDIO_MODE]      =   {0x08,  0},

    [PARAM_AUDIO_GAIN0]     =   {0x18,  0},
    [PARAM_AUDIO_GAIN1]     =   {0x19,  0},
    [PARAM_AUDIO_GAIN2]     =   {0x1A,  0},
    [PARAM_AUDIO_GAIN3]     =   {0x1B,  0},
    [PARAM_AUDIO_GAIN4]     =   {0x1C,  0},
    [PARAM_AUDIO_GAIN5]     =   {0x1D,  0},
    [PARAM_AUDIO_GAIN6]     =   {0x1E,  0},
    [PARAM_AUDIO_GAIN7]     =   {0x1F,  0},

    [PARAM_AUDIO_VOLUME]    =   {0x20,  0},
    [PARAM_AUDIO_BASS]      =   {0x21,  0},
    [PARAM_AUDIO_MIDDLE]    =   {0x22,  0},
    [PARAM_AUDIO_TREBLE]    =   {0x23,  0},
    [PARAM_AUDIO_FRONTREAR] =   {0x24,  0},
    [PARAM_AUDIO_BALANCE]   =   {0x25,  0},
    [PARAM_AUDIO_CENTER]    =   {0x26,  0},
    [PARAM_AUDIO_SUBWOOFER] =   {0x27,  0},
    [PARAM_AUDIO_PREAMP]    =   {0x28,  0},

    [PARAM_TUNER_IC]        =   {0x30,  0},
    [PARAM_TUNER_BAND]      =   {0x31,  0},
    [PARAM_TUNER_STEP]      =   {0x32,  0},
    [PARAM_TUNER_DEEMPH]    =   {0x33,  0},
    [PARAM_TUNER_STA_MODE]  =   {0x34,  false},
    [PARAM_TUNER_FMONO]     =   {0x35,  false},
    [PARAM_TUNER_RDS]       =   {0x36,  true},
    [PARAM_TUNER_BASS]      =   {0x37,  false},
    [PARAM_TUNER_VOLUME]    =   {0x38,  0},
    [PARAM_TUNER_FREQ]      =   {0x39,  9950},

    [PARAM_TUNER_FAV_0]     =   {0x40,  0},
    [PARAM_TUNER_FAV_1]     =   {0x41,  0},
    [PARAM_TUNER_FAV_2]     =   {0x42,  0},
    [PARAM_TUNER_FAV_3]     =   {0x43,  0},
    [PARAM_TUNER_FAV_4]     =   {0x44,  0},
    [PARAM_TUNER_FAV_5]     =   {0x45,  0},
    [PARAM_TUNER_FAV_6]     =   {0x46,  0},
    [PARAM_TUNER_FAV_7]     =   {0x47,  0},
    [PARAM_TUNER_FAV_8]     =   {0x48,  0},
    [PARAM_TUNER_FAV_9]     =   {0x49,  0},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  0},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},
    [PARAM_SPECTRUM_GRAD]   =   {0x52,  false},
    [PARAM_SPECTRUM_DEMO]   =   {0x53,  false},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  0},

    [PARAM_SYSTEM_LANG]     =   {0x70,  0},
    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  4},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
};

void settingsInit(void)
{
    eeInit(eeMap, PARAM_END);
}

int16_t settingsRead(Param param, int16_t defValue)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return (int16_t)EE_NOT_FOUND;
    }

    uint16_t value = eeReadRaw(eeMap[param].cell);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    return defValue;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param].cell, (uint16_t)value);
}

const EE_Cell *eeMapGet(void)
{
    return eeMap;
}
