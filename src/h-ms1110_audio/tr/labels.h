#ifndef TR_H
#define TR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "audio/audiodefs.h"
//#include "gui/palette.h"
#include "menu.h"
#include "pins.h"
#include "rtc.h"
#include "screen.h"
//#include "spectrum.h"
//#include "tuner/tuner.h"

typedef uint8_t Lang;
enum {
    LANG_DEFAULT = 0,

    LANG_BY,
    LANG_FR,
    LANG_RO,
    LANG_RU,
    LANG_TR,
    LANG_UA,

    LANG_END
};

typedef enum {
    LABEL_SUNDAY,
    LABEL_MONDAY,
    LABEL_TUESDAY,
    LABEL_WEDNESDAY,
    LABEL_THURSDAY,
    LABEL_FRIDAY,
    LABEL_SATURDAY,

    LABEL_VOLUME,
    LABEL_BASS,
    LABEL_MIDDLE,
    LABEL_TREBLE,
    LABEL_FRONTREAR,
    LABEL_BALANCE,
    LABEL_CENTER,
    LABEL_SUBWOOFER,
    LABEL_PREAMP,

    LABEL_IN_TUNER,
    LABEL_IN_PC,
    LABEL_IN_TV,
    LABEL_IN_BLUETOOTH,
    LABEL_IN_DVD,
    LABEL_IN_USB,
    LABEL_IN_MICROPHONE,
    LABEL_IN_GUITAR,
    LABEL_IN_TURNTABLES,
    LABEL_IN_CASSETTE,
    LABEL_IN_PROJECTOR,
    LABEL_IN_SATELLITE,
    LABEL_IN_MIXER,
    LABEL_IN_KARADIO,

    LABEL_BOOL_OFF,
    LABEL_BOOL_ON,

    LABEL_AUDIO_IC,
    LABEL_AUDIO_IC_END = LABEL_AUDIO_IC + (AUDIO_IC_END - AUDIO_IC_NO),

    LABEL_ALARM_DAY = LABEL_AUDIO_IC_END,
    LABEL_ALARM_DAY_END = LABEL_ALARM_DAY + (ALARM_DAY_END - ALARM_DAY_OFF),

    // Menu
    LABEL_MENU = LABEL_ALARM_DAY_END,
    LABEL_MENU_END = LABEL_MENU + (MENU_END - MENU_NULL),

    LABEL_END = LABEL_MENU_END,
} Label;

//extern const char *const labels_by[LABEL_END];
//extern const char *const labels_fr[LABEL_END];
//extern const char *const labels_ru[LABEL_END];
//extern const char *const labels_tr[LABEL_END];
//extern const char *const labels_ua[LABEL_END];
//extern const char *const labels_ro[LABEL_END];

void labelsSetLang(Lang value);
Lang labelsGetLang(void);
const char *labelsGetLangName(Lang value);

const char *labelsGet(Label value);

void labelsInit(void);

#ifdef __cplusplus
}
#endif

#endif // TR_H
