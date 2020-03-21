#ifndef TR_H
#define TR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "amp.h"
#include "gui/palette.h"
#include "menu.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"
#include "tuner/tuner.h"

#define GENERATE_MENU_RC_TEXT(CMD)  [LABEL_MENU + MENU_RC_ ## CMD] = # CMD,
#define GENERATE_TUNER_IC_TEXT(IC)  [LABEL_TUNER_IC + TUNER_IC_ ## IC] = # IC,

typedef uint8_t Lang;
enum {
    LANG_EN,
//    LANG_BY,
//    LANG_RU,

    LANG_END,

    LANG_DEFAULT = LANG_EN,
};

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_BOOL_OFF,
    LABEL_BOOL_ON,

    LABEL_TUNER_FM_STATION_NAME,

    LABEL_TUNER_IC,
    LABEL_TUNER_IC_END = LABEL_TUNER_IC + (TUNER_IC_END - TUNER_IC_NO),

    LABEL_TUNER_BAND = LABEL_TUNER_IC_END,
    LABEL_TUNER_BAND_END = LABEL_TUNER_BAND + (TUNER_BAND_END - TUNER_BAND_FM_US_EUROPE),

    LABEL_TUNER_STEP = LABEL_TUNER_BAND_END,
    LABEL_TUNER_STEP_END = LABEL_TUNER_STEP + (TUNER_STEP_END - TUNER_STEP_50K),

    LABEL_TUNER_DEEMPH = LABEL_TUNER_STEP_END,
    LABEL_TUNER_DEEMPH_END = LABEL_TUNER_DEEMPH + (TUNER_DEEMPH_END - TUNER_DEEMPH_50u),

    // Menu
    LABEL_MENU = LABEL_TUNER_DEEMPH_END,
    LABEL_MENU_END = LABEL_MENU + (MENU_END - MENU_NULL),

    LABEL_END = LABEL_MENU_END,
} Label;

extern const char *const labels_en[LABEL_END];
//extern const char *const labels_by[LABEL_END];
//extern const char *const labels_ru[LABEL_END];

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
