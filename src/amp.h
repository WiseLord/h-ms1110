#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_INIT,

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

void ampActionGet(void);

void ampActionHandle(void);

void ampShowScreen(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
