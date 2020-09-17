#include "settings.h"

#include "amp.h"
#include "display/glcd.h"
#include "eemul.h"
#include "gui/canvas.h"
#include "input.h"
#include "rc.h"
#include "spectrum.h"
#include "tr/labels.h"
#include "tuner/tuner.h"
#include "tuner/tunerdefs.h"

static int16_t rtcCorr = 0;

static const EE_Cell eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_TUNER_IC]        =   {0x30,  TUNER_IC_LC7213X},
    [PARAM_TUNER_BAND]      =   {0x31,  TUNER_BAND_FM_US_EUROPE},
    [PARAM_TUNER_STEP]      =   {0x32,  TUNER_STEP_100K},
    [PARAM_TUNER_DEEMPH]    =   {0x33,  TUNER_DEEMPH_50u},
    [PARAM_TUNER_STA_MODE]  =   {0x34,  false},
    [PARAM_TUNER_FMONO]     =   {0x35,  false},
    [PARAM_TUNER_RDS]       =   {0x36,  true},
    [PARAM_TUNER_BASS]      =   {0x37,  false},
    [PARAM_TUNER_VOLUME]    =   {0x38,  TUNER_VOLUME_MAX},
    [PARAM_TUNER_FREQ]      =   {0x39,  9950},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  SP_MODE_LEFT},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},
    [PARAM_SPECTRUM_GRAD]   =   {0x52,  false},

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

    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();

    switch (param) {

    case PARAM_TUNER_IC:
        ret = tuner->par.ic;
        break;
    case PARAM_TUNER_BAND:
        ret = tuner->par.band;
        break;
    case PARAM_TUNER_STEP:
        ret = tuner->par.step;
        break;
    case PARAM_TUNER_DEEMPH:
        ret = tuner->par.deemph;
        break;
    case PARAM_TUNER_STA_MODE:
        ret = tuner->par.stationMode;
        break;
    case PARAM_TUNER_FMONO:
        ret = tuner->par.forcedMono;
        break;
    case PARAM_TUNER_RDS:
        ret = tuner->par.rds;
        break;
    case PARAM_TUNER_BASS:
        ret = tuner->par.bassBoost;
        break;
    case PARAM_TUNER_VOLUME:
        ret = tuner->par.volume;
        break;
    case PARAM_TUNER_FREQ:
        ret = (int16_t)tuner->status.freq;
        break;

    case PARAM_SPECTRUM_MODE:
        ret = sp->mode;
        break;
    case PARAM_SPECTRUM_PEAKS:
        ret = sp->peaks;
        break;
    case PARAM_SPECTRUM_GRAD:
        ret = sp->grad;
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
    Tuner *tuner = tunerGet();
    Spectrum *sp = spGet();

    switch (param) {

    case PARAM_TUNER_IC:
        tuner->par.ic = (TunerIC)value;
        break;
    case PARAM_TUNER_BAND:
        tuner->par.band = (TunerBand)value;
        break;
    case PARAM_TUNER_STEP:
        tuner->par.step = (TunerStep)value;
        break;
    case PARAM_TUNER_DEEMPH:
        tuner->par.deemph = (TunerDeemph)value;
        break;
    case PARAM_TUNER_STA_MODE:
        tuner->par.stationMode = (bool)value;
        break;
    case PARAM_TUNER_FMONO:
        tuner->par.forcedMono = (bool)value;
        break;
    case PARAM_TUNER_RDS:
        tuner->par.rds = (bool)value;
        break;
    case PARAM_TUNER_BASS:
        tuner->par.bassBoost = (bool)value;
        break;
    case PARAM_TUNER_VOLUME:
        tuner->par.volume = (int8_t)value;
        break;
    case PARAM_TUNER_FREQ:
        tuner->status.freq = (uint16_t)value;
        break;

    case PARAM_SPECTRUM_MODE:
        sp->mode = (SpMode)value;
        break;
    case PARAM_SPECTRUM_PEAKS:
        sp->peaks = (bool)value;
        break;
    case PARAM_SPECTRUM_GRAD:
        sp->grad = (bool)value;
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
