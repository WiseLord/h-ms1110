#include "labels.h"

const char *const labels_en[LABEL_END] = {
    [LABEL_SUNDAY]          = "SUNDAY",
    [LABEL_MONDAY]          = "MONDAY",
    [LABEL_TUESDAY]         = "TUESDAY",
    [LABEL_WEDNESDAY]       = "WEDNESDAY",
    [LABEL_THURSDAY]        = "THURSDAY",
    [LABEL_FRIDAY]          = "FRIDAY",
    [LABEL_SATURDAY]        = "SATURDAY",

    [LABEL_VOLUME]          = "Volume",
    [LABEL_BASS]            = "Bass",
    [LABEL_MIDDLE]          = "Middle",
    [LABEL_TREBLE]          = "Treble",
    [LABEL_FRONTREAR]       = "Front/rear",
    [LABEL_BALANCE]         = "Balance",
    [LABEL_CENTER]          = "Center",
    [LABEL_SUBWOOFER]       = "Subwoofer",
    [LABEL_PREAMP]          = "Preamp",

    [LABEL_IN_TUNER]        = "Tuner",
    [LABEL_IN_PC]           = "Computer",
    [LABEL_IN_TV]           = "TV set",
    [LABEL_IN_BLUETOOTH]    = "Bluetooth",
    [LABEL_IN_DVD]          = "DVD",
    [LABEL_IN_USB]          = "USB",
    [LABEL_IN_MICROPHONE]   = "Microphone",
    [LABEL_IN_GUITAR]       = "Guitar",
    [LABEL_IN_TURNTABLES]   = "Turntables",
    [LABEL_IN_SDCARD]       = "SD card",
    [LABEL_IN_PROJECTOR]    = "Projector",
    [LABEL_IN_SATELLITE]    = "Satellite",
    [LABEL_IN_MIXER]        = "Mixer",
    [LABEL_IN_KARADIO]      = "KaRadio",

    [LABEL_BOOL_OFF]        = "OFF",
    [LABEL_BOOL_ON]         = "ON",

    [LABEL_SPECTRUM_MODE + SP_MODE_STEREO]      = "Stereo",
    [LABEL_SPECTRUM_MODE + SP_MODE_MIXED]       = "Mixed",
    [LABEL_SPECTRUM_MODE + SP_MODE_WATERFALL]   = "Waterfall",

    FOREACH_AUDIO_IC(GENERATE_AUDIO_IC_TEXT)

    [LABEL_ALARM_DAY + ALARM_DAY_OFF]       = "Disabled",
    [LABEL_ALARM_DAY + ALARM_DAY_WEEKDAYS]  = "Weekdays",
    [LABEL_ALARM_DAY + ALARM_DAY_ALL_DAYS]  = "All days",

    // NOTE: Keep in sync with MenuIdx in menu.h
    [LABEL_MENU + MENU_NULL]            = "Up menu",

    [LABEL_MENU + MENU_SETUP]           = "Settings",

    [LABEL_MENU + MENU_SETUP_SYSTEM]    = "System",
    [LABEL_MENU + MENU_SETUP_AUDIO]     = "Audio",
    [LABEL_MENU + MENU_SETUP_SPECTRUM]  = "Spectrum",
    [LABEL_MENU + MENU_SETUP_DISPLAY]   = "Display",
    [LABEL_MENU + MENU_SETUP_ALARM]     = "Alarm",
    [LABEL_MENU + MENU_SETUP_RC]        = "Remote",

    [LABEL_MENU + MENU_SYSTEM_LANG]     = "Language",
    [LABEL_MENU + MENU_SYSTEM_ENC_RES]  = "Encoder resolution",
    [LABEL_MENU + MENU_SYSTEM_SIL_TIM]  = "Silence timer",
    [LABEL_MENU + MENU_SYSTEM_RTC_CORR] = "Time correction",

    [LABEL_MENU + MENU_AUDIO_IC]        = "Audioproc",
    [LABEL_MENU + MENU_AUDIO_IN]        = "Input",

    [LABEL_MENU + MENU_ALARM_HOUR]      = "Hour",
    [LABEL_MENU + MENU_ALARM_MINUTE]    = "Minute",
    [LABEL_MENU + MENU_ALARM_DAYS]      = "Days",

    [LABEL_MENU + MENU_SPECTRUM_MODE]   = "Display mode",
    [LABEL_MENU + MENU_SPECTRUM_PEAKS]  = "Peak level",
    [LABEL_MENU + MENU_DISPLAY_DEF]     = "Main screen",

    // NOTE: Keep in sync with cmd.h define list
    [LABEL_MENU + MENU_RC_STBY_SWITCH]  = "Switch standby",
    [LABEL_MENU + MENU_RC_MUTE]         = "Mute",
    [LABEL_MENU + MENU_RC_VOL_UP]       = "Volume up",
    [LABEL_MENU + MENU_RC_VOL_DOWN]     = "Volume down",
    [LABEL_MENU + MENU_RC_MENU]         = "Menu",
    [LABEL_MENU + MENU_RC_CHAN_NEXT]    = "Next chan",
    [LABEL_MENU + MENU_RC_CHAN_PREV]    = "Prev chan",
    [LABEL_MENU + MENU_RC_DIG]          = "Digit",
    [LABEL_MENU + MENU_RC_IN_NEXT]      = "Next input",
    [LABEL_MENU + MENU_RC_NAV_OK]       = "OK",
    [LABEL_MENU + MENU_RC_NAV_BACK]     = "Back",
    [LABEL_MENU + MENU_RC_NAV_RIGHT]    = "Right",
    [LABEL_MENU + MENU_RC_NAV_UP]       = "Up",
    [LABEL_MENU + MENU_RC_NAV_LEFT]     = "Left",
    [LABEL_MENU + MENU_RC_NAV_DOWN]     = "Down",
    [LABEL_MENU + MENU_RC_BASS_UP]      = "Bass up",
    [LABEL_MENU + MENU_RC_BASS_DOWN]    = "Bass down",
    [LABEL_MENU + MENU_RC_MIDDLE_UP]    = "Middle up",
    [LABEL_MENU + MENU_RC_MIDDLE_DOWN]  = "Middle down",
    [LABEL_MENU + MENU_RC_TREBLE_UP]    = "Treble up",
    [LABEL_MENU + MENU_RC_TREBLE_DOWN]  = "Treble down",
    [LABEL_MENU + MENU_RC_LOUDNESS]     = "Loudness",
    [LABEL_MENU + MENU_RC_SURROUND]     = "Surround",
    [LABEL_MENU + MENU_RC_EFFECT_3D]    = "3D-effect",
    [LABEL_MENU + MENU_RC_TONE_BYPASS]  = "Bypass tone",
    [LABEL_MENU + MENU_RC_TIME]         = "Time",
    [LABEL_MENU + MENU_RC_STOP]         = "Stop",
    [LABEL_MENU + MENU_RC_PLAY_PAUSE]   = "Play/pause",
    [LABEL_MENU + MENU_RC_REW]          = "Rewind",
    [LABEL_MENU + MENU_RC_FWD]          = "Flash forward",
    [LABEL_MENU + MENU_RC_TIMER]        = "Timer",
    [LABEL_MENU + MENU_RC_SP_MODE]      = "Spectrum mode",
    [LABEL_MENU + MENU_RC_SCR_DEF]      = "Default screen",
    [LABEL_MENU + MENU_RC_BT_IN_NEXT]   = "Next BT input",
};