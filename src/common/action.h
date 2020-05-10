#ifndef ACTION_H
#define ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FOREACH_CMD(CMD)    \
    CMD(STBY_SWITCH)        \
    CMD(MUTE)               \
    CMD(VOL_UP)             \
    CMD(VOL_DOWN)           \
    CMD(MENU)               \
    CMD(CHAN_PREV)          \
    CMD(CHAN_NEXT)          \
    CMD(DIG_0)              \
    CMD(DIG_1)              \
    CMD(DIG_2)              \
    CMD(DIG_3)              \
    CMD(DIG_4)              \
    CMD(DIG_5)              \
    CMD(DIG_6)              \
    CMD(DIG_7)              \
    CMD(DIG_8)              \
    CMD(DIG_9)              \
    CMD(IN_PREV)            \
    CMD(IN_NEXT)            \
    CMD(NAV_OK)             \
    CMD(NAV_BACK)           \
    CMD(NAV_RIGHT)          \
    CMD(NAV_UP)             \
    CMD(NAV_LEFT)           \
    CMD(NAV_DOWN)           \
    CMD(BASS_UP)            \
    CMD(BASS_DOWN)          \
    CMD(MIDDLE_UP)          \
    CMD(MIDDLE_DOWN)        \
    CMD(TREBLE_UP)          \
    CMD(TREBLE_DOWN)        \
    CMD(LOUDNESS)           \
    CMD(TIME)               \
    CMD(STOP)               \
    CMD(PLAY_PAUSE)         \
    CMD(REW)                \
    CMD(FWD)                \
    CMD(TIMER)              \
    CMD(SP_MODE)            \
    CMD(SCR_DEF)            \

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,

    ACTION_TUNER_BTN_SHORT,
    ACTION_TUNER_BTN_LONG,
    ACTION_TUNER_ENCODER,

    ACTION_INIT_HW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_RESTORE_VOLUME,
    ACTION_DISP_EXPIRED,

    ACTION_SETUP_SELECT,
    ACTION_SETUP_SWITCH_CHILD,
    ACTION_SETUP_CHANGE_CHILD,
    ACTION_SETUP_BACK,

    ACTION_REMOTE,

    ACTION_TUNER_SET_FREQ,


    ACTION_OPEN_MENU,

    ACTION_AUDIO_INPUT,
    ACTION_AUDIO_PARAM_CHANGE,
    ACTION_AUDIO_PARAM_SET,

    ACTION_SP_MODE,

    ACTION_END
};

typedef uint8_t ScreenType;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,
    SCREEN_INPUT,

    SCREEN_STANDBY,
    SCREEN_TUNE,
    SCREEN_SETUP,

    SCREEN_TUNER,

    SCREEN_END
};

typedef uint8_t InputType;
enum {
    IN_TUNER = 0,
    IN_PC,
    IN_TV,
    IN_BLUETOOTH,
    IN_DVD,
    IN_USB,
    IN_MICROPHONE,
    IN_GUITAR,
    IN_TURNTABLES,
    IN_SDCARD,
    IN_PROJECTOR,
    IN_SATELLITE,
    IN_MIXER,
    IN_KARADIO,

    IN_END
};

typedef struct {
    ActionType type;
    ScreenType screen;

    int16_t value;
    int16_t timeout;
} Action;

#ifdef __cplusplus
}
#endif

#endif // ACTION_H
