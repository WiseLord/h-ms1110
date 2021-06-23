#include "labels.h"

#define STR(str) #str

#define GENERATE_LABEL_STRING(RC)  [LABEL_RC_ ## RC] = STR(RC),

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
    [LABEL_BALANCE]         = "Balance",
    [LABEL_FRONTREAR]       = "Front/rear",
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
    [LABEL_SETUP_TIMECORR]  = "Time correction",
    [LABEL_SETUP_ALARM]     = "Alarm",
    [LABEL_SETUP_REMOTE]    = "Remote",

    [LABEL_ALARM_DAY_OFF]       = "disabled",
    [LABEL_ALARM_DAY_WEEKDAYS]  = "weekdays",
    [LABEL_ALARM_DAY_ALL_DAYS]  = "all days",

    [LABEL_MPD_WAIT]        = "Waiting for MPD...",
    [LABEL_BT_WAIT]         = "Waiting for Bluetooth...",

    FOREACH_CMD(GENERATE_LABEL_STRING)
};
