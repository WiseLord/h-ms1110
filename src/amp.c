#include "amp.h"

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_TYPE_END
};

typedef uint8_t ScreenMode;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,

    SCREEN_STANDBY,

    SCREEN_END
};

typedef struct {
    ScreenMode mode;
    ScreenMode def;
} Screen;

typedef struct {
    ActionType type;
    ScreenMode screen;

    int16_t value;
    int16_t timeout;
} Action;
