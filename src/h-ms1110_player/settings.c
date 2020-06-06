#include "settings.h"

#include "amp.h"
#include "display/glcd.h"
#include "eemul.h"
#include "gui/canvas.h"
#include "input.h"
#include "rc.h"
#include "spectrum.h"
#include "tr/labels.h"

static int16_t rtcCorr = 0;

static const EE_Cell eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_DISPLAY_DEF]     =   {0x43,  SCREEN_SPECTRUM},

    [PARAM_SYSTEM_LANG]     =   {0x70,  LANG_RU},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  -4},
};

void settingsInit(void)
{
    eeInit(eeMap, PARAM_END);

    for (Param par = PARAM_NULL + 1; par < PARAM_END; par++) {
        settingsSet(par, settingsRead(par));
    }
}

int16_t settingsGet(Param param)
{
    int16_t ret = 0;

    switch (param) {

    case PARAM_DISPLAY_DEF:
        ret = ampGet()->defScreen;
        break;

    case PARAM_SYSTEM_LANG:
        ret = labelsGetLang();
        break;
    case PARAM_SYSTEM_ENC_RES:
        ret = inputGet()->encRes;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcCorr;
        break;

    default:
        break;
    }

    return  ret;
}

void settingsSet(Param param, int16_t value)
{
    switch (param) {

    case PARAM_DISPLAY_DEF:
        ampGet()->defScreen = (ScreenType)value;
        break;

    case PARAM_SYSTEM_LANG:
        labelsSetLang((Lang)value);
        break;
    case PARAM_SYSTEM_ENC_RES:
        inputGet()->encRes = (int8_t)value;
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
    return eeMap[param].value;
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
