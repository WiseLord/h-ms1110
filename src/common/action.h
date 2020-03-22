#ifndef ACTION_H
#define ACTION_H

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    ACTION_INIT_HW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_DISP_EXPIRED,

    ACTION_OPEN_MENU,

    ACTION_AUDIO_INPUT,
    ACTION_AUDIO_PARAM_CHANGE,
    ACTION_AUDIO_PARAM_SET,

    ACTION_AUDIO_MUTE,
    ACTION_AUDIO_LOUDNESS,
    ACTION_AUDIO_BYPASS,

    ACTION_RESTORE_VOLUME,

    ACTION_MENU_SELECT,
    ACTION_MENU_CHANGE,

    ACTION_END
};

typedef uint8_t SyncType;
enum {
    SYNC_NONE = 0,

    SYNC_ACTION,
    SYNC_DISPLAY,

    SYNC_END
};

typedef struct {
    int16_t value;
    ActionType type;
} SyncAction;

typedef union {
    uint8_t data[8];
    struct {
        SyncType type;
        union {
            SyncAction action;
        };
    };
} AmpSync;

#endif // ACTION_H
