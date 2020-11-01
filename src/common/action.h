#ifndef ACTION_H
#define ACTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "spectrum.h"

#define FOREACH_CMD(CMD)    \
    CMD(STBY_SWITCH)        \
    CMD(VOL_UP)             \
    CMD(VOL_DOWN)           \
    CMD(MUTE)               \
                            \
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

    // Actions to be remapped

    ACTION_PLAYER_BTN_SHORT,
    ACTION_PLAYER_BTN_LONG,
    ACTION_PLAYER_ENCODER,

    ACTION_TUNER_BTN_SHORT,
    ACTION_TUNER_BTN_LONG,
    ACTION_TUNER_ENCODER,

    ACTION_SPECTRUM_BTN_SHORT,
    ACTION_SPECTRUM_BTN_LONG,

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

    ACTION_TUNER_SET_FREQ = 80,

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
