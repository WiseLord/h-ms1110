#include "labels.h"

const char *const labels_en[LABEL_END] = {
    [LABEL_SUNDAY]          = "Sunday",
    [LABEL_MONDAY]          = "Monday",
    [LABEL_TUESDAY]         = "Tuesday",
    [LABEL_WEDNESDAY]       = "Wednesday",
    [LABEL_THURSDAY]        = "Thursday",
    [LABEL_FRIDAY]          = "Friday",
    [LABEL_SATURDAY]        = "Saturday",

    [LABEL_JANUARY]         = "January",
    [LABEL_FEBRUARY]        = "February",
    [LABEL_MARCH]           = "March",
    [LABEL_APRIL]           = "April",
    [LABEL_MAY]             = "May",
    [LABEL_JUNE]            = "June",
    [LABEL_JULY]            = "July",
    [LABEL_AUGUST]          = "August",
    [LABEL_SEPTEMBER]       = "September",
    [LABEL_OCTOBER]         = "October",
    [LABEL_NOVEMBER]        = "November",
    [LABEL_DECEMBER]        = "December",

    [LABEL_VOLUME]          = "Volume",
    [LABEL_BASS]            = "Bass",
    [LABEL_MIDDLE]          = "Middle",
    [LABEL_TREBLE]          = "Treble",
    [LABEL_FRONTREAR]       = "Front/rear",
    [LABEL_BALANCE]         = "Balance",
    [LABEL_CENTER]          = "Center",
    [LABEL_SUBWOOFER]       = "Subwoofer",
    [LABEL_PREAMP]          = "Preamp",
    [LABEL_LOUDNESS]        = "Loudness",

    [LABEL_BASS_FREQ]       = "Bass freq",
    [LABEL_BASS_QUAL]       = "Bass Q",
    [LABEL_MIDDLE_KFREQ]    = "Middle freq",
    [LABEL_MIDDLE_QUAL]     = "Middle Q",
    [LABEL_TREBLE_KFREQ]    = "Treble freq",
    [LABEL_SUB_CUT_FREQ]    = "Sub cut freq",
    [LABEL_LOUD_PEAK_FREQ]  = "Loudness freq",

    [LABEL_IN_TUNER]        = "Tuner",
    [LABEL_IN_MPD]          = "MPD",
    [LABEL_IN_AUX1]         = "AUX1",
    [LABEL_IN_AUX2]         = "AUX2",
    [LABEL_IN_SPDIF]        = "SPDIF",

    [LABEL_BOOL_OFF]        = "OFF",
    [LABEL_BOOL_ON]         = "ON",

    [LABEL_DB]              = "dB",
    [LABEL_HZ]              = "Hz",
    [LABEL_K]               = "k",

    [LABEL_SETUP_MAIN]      = "Setup",
    [LABEL_SETUP_TIME]      = "Time",
    [LABEL_SETUP_DATE]      = "Date",
    [LABEL_SETUP_ALARM]     = "Alarm",
    [LABEL_SETUP_REMOTE]    = "Remote",

    [LABEL_ALARM_DAY_OFF]       = "disabled",
    [LABEL_ALARM_DAY_WEEKDAYS]  = "weekdays",
    [LABEL_ALARM_DAY_ALL_DAYS]  = "all days",

    [LABEL_MPD_WAIT]        = "Waiting for MPD...",

    // NOTE: Keep in sync with cmd.h define list
    [LABEL_RC_STBY_SWITCH]  = "Switch standby",
    [LABEL_RC_VOL_UP]       = "Volume up",
    [LABEL_RC_VOL_DOWN]     = "Volume down",
    [LABEL_RC_MUTE]         = "Mute",

    [LABEL_RC_MENU]         = "Menu",
    [LABEL_RC_CHAN_PREV]    = "Prev chan",
    [LABEL_RC_CHAN_NEXT]    = "Next chan",
    [LABEL_RC_DIG_0]        = "Digit 0",
    [LABEL_RC_DIG_1]        = "Digit 1",
    [LABEL_RC_DIG_2]        = "Digit 2",
    [LABEL_RC_DIG_3]        = "Digit 3",
    [LABEL_RC_DIG_4]        = "Digit 4",
    [LABEL_RC_DIG_5]        = "Digit 5",
    [LABEL_RC_DIG_6]        = "Digit 6",
    [LABEL_RC_DIG_7]        = "Digit 7",
    [LABEL_RC_DIG_8]        = "Digit 8",
    [LABEL_RC_DIG_9]        = "Digit 9",
    [LABEL_RC_IN_PREV]      = "Previous input",
    [LABEL_RC_IN_NEXT]      = "Next input",
    [LABEL_RC_NAV_OK]       = "OK",
    [LABEL_RC_NAV_BACK]     = "Back",
    [LABEL_RC_NAV_RIGHT]    = "Right",
    [LABEL_RC_NAV_UP]       = "Up",
    [LABEL_RC_NAV_LEFT]     = "Left",
    [LABEL_RC_NAV_DOWN]     = "Down",
    [LABEL_RC_BASS_UP]      = "Bass up",
    [LABEL_RC_BASS_DOWN]    = "Bass down",
    [LABEL_RC_MIDDLE_UP]    = "Middle up",
    [LABEL_RC_MIDDLE_DOWN]  = "Middle down",
    [LABEL_RC_TREBLE_UP]    = "Treble up",
    [LABEL_RC_TREBLE_DOWN]  = "Treble down",
    [LABEL_RC_LOUDNESS]     = "Loudness",
    [LABEL_RC_TIME]         = "Time",
    [LABEL_RC_STOP]         = "Stop",
    [LABEL_RC_PLAY]         = "Play",
    [LABEL_RC_PAUSE]        = "Pause",
    [LABEL_RC_PREV]         = "Previous",
    [LABEL_RC_NEXT]         = "Next",
    [LABEL_RC_REW]          = "Rewind",
    [LABEL_RC_FWD]          = "Flash forward",
    [LABEL_RC_TIMER]        = "Timer",
    [LABEL_RC_SP_MODE]      = "Spectrum mode",
    [LABEL_RC_SCR_DEF]      = "Default screen",
};
