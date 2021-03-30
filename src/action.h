#ifndef ACTION_H
#define ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FOREACH_CMD(CMD)    \
    CMD(STBY_SWITCH)        \
    CMD(MUTE)               \
    CMD(IN_NEXT)            \
    CMD(VOL_UP)             \
    CMD(VOL_DOWN)           \
    CMD(CHAN_PREV)          \
    CMD(CHAN_NEXT)          \
    CMD(AUDIO_MENU)         \
    CMD(AUDIO_SUBMENU)      \
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
    CMD(NAV_LEFT)           \
    CMD(NAV_RIGHT)          \
    CMD(NAV_UP)             \
    CMD(NAV_DOWN)           \
    CMD(NAV_OK)             \
    CMD(NAV_BACK)           \
    CMD(TIME)               \
    CMD(PLAY)               \
    CMD(PAUSE)              \
    CMD(STOP)               \
    CMD(REW)                \
    CMD(FWD)                \
    CMD(PREV)               \
    CMD(NEXT)               \
    CMD(TIMER)              \
    CMD(SP_MODE)            \
    CMD(SP_PEAKS)           \
    CMD(SP_DEMO)            \

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    // Actions to be remapped

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    // Actions to be handled

    ACTION_INIT_HW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_RESTORE_VOLUME,
    ACTION_DISP_EXPIRED,

    ACTION_SETUP_SELECT,
    ACTION_SETUP_SWITCH_CHILD,
    ACTION_SETUP_CHANGE_CHILD,
    ACTION_SETUP_BACK,

    ACTION_AUDIO_MENU,
    ACTION_AUDIO_SELECT_INPUT,
    ACTION_AUDIO_SELECT_PARAM,
    ACTION_AUDIO_SET_PARAM,

    ACTION_AUDIO_MUTE,

    ACTION_SP_CHANGE_MODE = 60,
    ACTION_SP_CHANGE_PEAKS,
    ACTION_SP_CHANGE_DEMO,

    ACTION_TUNER_SET_FREQ = 80,
    ACTION_TUNER_STORE,
    ACTION_TUNER_FORCE_MONO,
    ACTION_TUNER_SET_RDS,

    ACTION_SWITCH_BAND    = 90,

    ACTION_DIGIT          = 100,
    ACTION_DIGIT_HOLD,

    ACTION_MEDIA          = 110,

    ACTION_MPD_POWEROFF   = 120,

    ACTION_END
};

typedef struct {
    ActionType type;
    int16_t value;
} Action;

#ifdef __cplusplus
}
#endif

#endif // ACTION_H
