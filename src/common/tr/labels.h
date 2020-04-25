#ifndef TR_H
#define TR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "amp.h"
#include "audio/audio.h"
#include "gui/palette.h"
#include "rtc.h"
#include "screen.h"
#include "spectrum.h"

#define GENERATE_MENU_RC_TEXT(CMD)  [LABEL_MENU + MENU_RC_ ## CMD] = # CMD,
#define GENERATE_AUDIO_IC_TEXT(IC)  [LABEL_AUDIO_IC + AUDIO_IC_ ## IC] = # IC,

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
    LABEL_IN_SDCARD,
    LABEL_IN_PROJECTOR,
    LABEL_IN_SATELLITE,
    LABEL_IN_MIXER,
    LABEL_IN_KARADIO,

    LABEL_BOOL_OFF,
    LABEL_BOOL_ON,

    LABEL_END,
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
