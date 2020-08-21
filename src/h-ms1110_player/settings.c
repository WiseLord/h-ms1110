#include "settings.h"

#include "amp.h"
#include "audio/audio.h"
#include "display/glcd.h"
#include "eemul.h"
#include "gui/canvas.h"
#include "input.h"
#include "rc.h"
#include "rtc.h"
#include "spectrum.h"
#include "tr/labels.h"

static uint8_t silenceTimer = 0;
static int16_t rtcCorr = 0;

#define GENERATE_EE_RC_MAP(CMD)  [PARAM_RC_ ## CMD] = {0x80 + RC_CMD_ ## CMD, (int16_t)EE_NOT_FOUND},

static const EE_Cell eeMap[] = {
    [PARAM_NULL]            =   {0x00,  0},

    [PARAM_AUDIO_IC]        =   {0x01,  AUDIO_IC_TDA7719},
    [PARAM_AUDIO_INPUT]     =   {0x02,  0},
    [PARAM_AUDIO_LOUDNESS]  =   {0x03,  false},
    [PARAM_AUDIO_SURROUND]  =   {0x04,  false},
    [PARAM_AUDIO_EFFECT3D]  =   {0x05,  false},
    [PARAM_AUDIO_BYPASS]    =   {0x06,  false},

    [PARAM_AUDIO_IN0]       =   {0x10,  IN_TUNER},
    [PARAM_AUDIO_IN1]       =   {0x11,  IN_END},
    [PARAM_AUDIO_IN2]       =   {0x12,  IN_END},
    [PARAM_AUDIO_IN3]       =   {0x13,  IN_END},
    [PARAM_AUDIO_IN4]       =   {0x14,  IN_PC},
    [PARAM_AUDIO_IN5]       =   {0x15,  IN_END},
    [PARAM_AUDIO_IN6]       =   {0x16,  IN_END},
    [PARAM_AUDIO_IN7]       =   {0x17,  IN_END},

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

    [PARAM_DISPLAY_DEF]     =   {0x43,  SCREEN_SPECTRUM},

    [PARAM_SPECTRUM_MODE]   =   {0x50,  SP_MODE_RIGHT},
    [PARAM_SPECTRUM_PEAKS]  =   {0x51,  true},
    [PARAM_SPECTRUM_GRAD]   =   {0x52,  false},

    [PARAM_ALARM_HOUR]      =   {0x60,  7},
    [PARAM_ALARM_MINUTE]    =   {0x61,  30},
    [PARAM_ALARM_DAYS]      =   {0x62,  ALARM_DAY_OFF},

    [PARAM_SYSTEM_LANG]     =   {0x70,  LANG_RU},
    [PARAM_SYSTEM_SIL_TIM]  =   {0x72,  5},
    [PARAM_SYSTEM_RTC_CORR] =   {0x73,  0},
    [PARAM_SYSTEM_ENC_RES]  =   {0x74,  -2},

    FOREACH_CMD(GENERATE_EE_RC_MAP)
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

    AudioProc *aProc = audioGet();
    Alarm *alarm = rtcGetAlarm(0);
    Amp *amp = ampGet();

    switch (param) {

    case PARAM_AUDIO_IC:
        ret = aProc->par.ic;
        break;
    case PARAM_AUDIO_INPUT:
        ret = aProc->par.input;
        break;
    case PARAM_AUDIO_LOUDNESS:
        ret = aProc->par.loudness;
        break;
    case PARAM_AUDIO_SURROUND:
        ret = aProc->par.surround;
        break;
    case PARAM_AUDIO_EFFECT3D:
        ret = aProc->par.effect3d;
        break;
    case PARAM_AUDIO_BYPASS:
        ret = aProc->par.bypass;
        break;

    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        ret = amp->inType[param - PARAM_AUDIO_IN0];
        break;

    case PARAM_AUDIO_GAIN0:
    case PARAM_AUDIO_GAIN1:
    case PARAM_AUDIO_GAIN2:
    case PARAM_AUDIO_GAIN3:
    case PARAM_AUDIO_GAIN4:
    case PARAM_AUDIO_GAIN5:
    case PARAM_AUDIO_GAIN6:
    case PARAM_AUDIO_GAIN7:
        ret = aProc->par.gain[param - PARAM_AUDIO_GAIN0];
        break;

    case PARAM_AUDIO_VOLUME:
    case PARAM_AUDIO_BASS:
    case PARAM_AUDIO_MIDDLE:
    case PARAM_AUDIO_TREBLE:
    case PARAM_AUDIO_FRONTREAR:
    case PARAM_AUDIO_BALANCE:
    case PARAM_AUDIO_CENTER:
    case PARAM_AUDIO_SUBWOOFER:
    case PARAM_AUDIO_PREAMP:
        ret = aProc->par.tune[param - PARAM_AUDIO_VOLUME].value;
        break;

    case PARAM_DISPLAY_DEF:
        ret = ampGet()->defScreen;
        break;

    case PARAM_SPECTRUM_MODE:
        ret = amp->sp.mode;
        break;
    case PARAM_SPECTRUM_PEAKS:
        ret = amp->sp.peaks;
        break;
    case PARAM_SPECTRUM_GRAD:
        ret = amp->sp.grad;
        break;

    case PARAM_ALARM_HOUR:
        ret = alarm->hour;
        break;
    case PARAM_ALARM_MINUTE:
        ret = alarm->min;
        break;
    case PARAM_ALARM_DAYS:
        ret = alarm->days;
        break;

    case PARAM_SYSTEM_LANG:
        ret = labelsGetLang();
        break;
    case PARAM_SYSTEM_ENC_RES:
        ret = inputGet()->encRes;
        break;
    case PARAM_SYSTEM_SIL_TIM:
        ret = silenceTimer;
        break;
    case PARAM_SYSTEM_RTC_CORR:
        ret = rtcCorr;
        break;

    default:
        break;
    }

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        ret = (int16_t)rcGetCode(param - PARAM_RC_STBY_SWITCH);
    }

    return  ret;
}

void settingsSet(Param param, int16_t value)
{
    AudioProc *aProc = audioGet();
    Alarm *alarm = rtcGetAlarm(0);
    Amp *amp = ampGet();

    switch (param) {

    case PARAM_AUDIO_IC:
        aProc->par.ic = (AudioIC)(value);
        break;
    case PARAM_AUDIO_INPUT:
        aProc->par.input = (int8_t)value;
        break;
    case PARAM_AUDIO_LOUDNESS:
        aProc->par.loudness = (int8_t)value;
        break;
    case PARAM_AUDIO_SURROUND:
        aProc->par.surround = (int8_t)value;
        break;
    case PARAM_AUDIO_EFFECT3D:
        aProc->par.effect3d = (int8_t)value;
        break;
    case PARAM_AUDIO_BYPASS:
        aProc->par.bypass = (int8_t)value;
        break;

    case PARAM_AUDIO_IN0:
    case PARAM_AUDIO_IN1:
    case PARAM_AUDIO_IN2:
    case PARAM_AUDIO_IN3:
    case PARAM_AUDIO_IN4:
    case PARAM_AUDIO_IN5:
    case PARAM_AUDIO_IN6:
    case PARAM_AUDIO_IN7:
        amp->inType[param - PARAM_AUDIO_IN0] = (InputType)value;
        break;

    case PARAM_AUDIO_GAIN0:
    case PARAM_AUDIO_GAIN1:
    case PARAM_AUDIO_GAIN2:
    case PARAM_AUDIO_GAIN3:
    case PARAM_AUDIO_GAIN4:
    case PARAM_AUDIO_GAIN5:
    case PARAM_AUDIO_GAIN6:
    case PARAM_AUDIO_GAIN7:
        aProc->par.gain[param - PARAM_AUDIO_GAIN0] = (int8_t)value;
        break;

    case PARAM_AUDIO_VOLUME:
    case PARAM_AUDIO_BASS:
    case PARAM_AUDIO_MIDDLE:
    case PARAM_AUDIO_TREBLE:
    case PARAM_AUDIO_FRONTREAR:
    case PARAM_AUDIO_BALANCE:
    case PARAM_AUDIO_CENTER:
    case PARAM_AUDIO_SUBWOOFER:
    case PARAM_AUDIO_PREAMP:
        aProc->par.tune[param - PARAM_AUDIO_VOLUME].value = (int8_t)value;
        break;

    case PARAM_DISPLAY_DEF:
        ampGet()->defScreen = (ScreenType)value;
        break;

    case PARAM_SPECTRUM_MODE:
        amp->sp.mode = (SpMode)value;
        break;
    case PARAM_SPECTRUM_PEAKS:
        amp->sp.peaks = (bool)value;
        break;
    case PARAM_SPECTRUM_GRAD:
        amp->sp.grad = (bool)value;
        break;

    case PARAM_ALARM_HOUR:
        alarm->hour = (int8_t)value;
        break;
    case PARAM_ALARM_MINUTE:
        alarm->min = (int8_t)value;
        break;
    case PARAM_ALARM_DAYS:
        alarm->days = (AlarmDay)value;
        break;

    case PARAM_SYSTEM_LANG:
        labelsSetLang((Lang)value);
        break;
    case PARAM_SYSTEM_ENC_RES:
        inputGet()->encRes = (int8_t)value;
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

    if (param >= PARAM_RC_STBY_SWITCH && param < PARAM_RC_STBY_SWITCH + RC_CMD_END) {
        rcSaveCode((uint16_t)(param - PARAM_RC_STBY_SWITCH), (uint16_t)value);
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