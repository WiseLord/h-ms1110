#include "settings.h"

#include "eemul.h"
#include "rtc.h"

static uint8_t silenceTimer = 0;
static int16_t rtcCorr = 0;

static const EE_Map eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  0},

    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  2},
};

void settingsInit(void)
{
    eeInit();

    settingsSet(PARAM_ALARM_HOUR, settingsRead(PARAM_ALARM_HOUR));
    settingsSet(PARAM_ALARM_MINUTE, settingsRead(PARAM_ALARM_MINUTE));
    settingsSet(PARAM_ALARM_DAYS, settingsRead(PARAM_ALARM_DAYS));

    settingsSet(PARAM_SYSTEM_SIL_TIM, settingsRead(PARAM_SYSTEM_SIL_TIM));
    settingsSet(PARAM_SYSTEM_RTC_CORR, settingsRead(PARAM_SYSTEM_RTC_CORR));
}

int16_t settingsGet(Param param)
{
    int16_t ret = 0;

    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case PARAM_ALARM_HOUR:
        ret = alarm->hour;
        break;
    case PARAM_ALARM_MINUTE:
        ret = alarm->min;
        break;
    case PARAM_ALARM_DAYS:
        ret = (int16_t)alarm->days;
        break;

    case PARAM_SYSTEM_SIL_TIM:
        ret = silenceTimer;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcCorr;
        break;

    case PARAM_SYSTEM_ENC_RES:
        ret = 2;
        break;
    default:
        break;
    }

    return  ret;
}

void settingsSet(Param param, int16_t value)
{
    Alarm *alarm = rtcGetAlarm(0);

    switch (param) {
    case PARAM_ALARM_HOUR:
        alarm->hour = (int8_t)value;
        break;
    case PARAM_ALARM_MINUTE:
        alarm->min = (int8_t)value;
        break;
    case PARAM_ALARM_DAYS:
        alarm->days = (AlarmDay)value;
        break;

    case PARAM_SYSTEM_SIL_TIM:
        silenceTimer = (uint8_t)value;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        rtcCorr = value;
        break;

    default:
        break;
    }
}

int16_t settingsRead(Param param)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return (int16_t)EE_NOT_FOUND;
    }

    uint16_t value = eeReadRaw(eeMap[param].cell);

    if (value != EE_NOT_FOUND) {
        return (int16_t)value;
    }

    // Return default value if not found
    return eeMap[param].def;
}

void settingsStore(Param param, int16_t value)
{
    if (param == PARAM_NULL || param >= PARAM_END) {
        return;
    }

    eeUpdateRaw(eeMap[param].cell, (uint16_t)value);
}

const EE_Map *eeMapGet(void)
{
    return eeMap;
}
