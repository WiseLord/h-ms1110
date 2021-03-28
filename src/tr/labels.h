#ifndef TR_H
#define TR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "action.h"

#define GENERATE_LABEL_RC_CMD(RC)    LABEL_RC_ ## RC,

typedef uint8_t Lang;
enum {
    LANG_EN,
//    LANG_BY,
    LANG_RU,

    LANG_END,

    LANG_DEFAULT = LANG_EN,
};

typedef uint8_t Label;
enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_JANUARY,
    LABEL_FEBRUARY,
    LABEL_MARCH,
    LABEL_APRIL,
    LABEL_MAY,
    LABEL_JUNE,
    LABEL_JULY,
    LABEL_AUGUST,
    LABEL_SEPTEMBER,
    LABEL_OCTOBER,
    LABEL_NOVEMBER,
    LABEL_DECEMBER,

    LABEL_VOLUME,
    LABEL_BASS,
    LABEL_MIDDLE,
    LABEL_TREBLE,
    LABEL_FRONTREAR,
    LABEL_BALANCE,
    LABEL_CENTER,
    LABEL_SUBWOOFER,
    LABEL_PREAMP,

    LABEL_BASS_FREQ,
    LABEL_BASS_QUAL,
    LABEL_MIDDLE_KFREQ,
    LABEL_MIDDLE_QUAL,
    LABEL_TREBLE_KFREQ,
    LABEL_SUB_CUT_FREQ,
    LABEL_LOUD_PEAK_FREQ,

    LABEL_IN_TUNER,
    LABEL_IN_MPD,
    LABEL_IN_AUX1,
    LABEL_IN_AUX2,
    LABEL_IN_SPDIF,

    LABEL_BOOL_OFF,
    LABEL_BOOL_ON,

    LABEL_SETUP_MAIN,
    LABEL_SETUP_TIME,
    LABEL_SETUP_DATE,
    LABEL_SETUP_ALARM,
    LABEL_SETUP_REMOTE,

    LABEL_ALARM_DAY_OFF,
    LABEL_ALARM_DAY_WEEKDAYS,
    LABEL_ALARM_DAY_ALL_DAYS,

    LABEL_MPD_WAIT,

    FOREACH_CMD(GENERATE_LABEL_RC_CMD)

    LABEL_END,
};

extern const char *const labels_en[LABEL_END];
//extern const char *const labels_by[LABEL_END];
extern const char *const labels_ru[LABEL_END];

void labelsSetLang(Lang value);
Lang labelsGetLang(void);
const char *labelsGetLangName(Lang value);

const char *labelsGetDefault(Label value);
const char *labelsGet(Label value);

void labelsInit(void);

#ifdef __cplusplus
}
#endif

#endif // TR_H
