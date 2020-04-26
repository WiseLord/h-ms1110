#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t ScreenType;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,

    SCREEN_STANDBY,

    SCREEN_END
};

typedef struct {
    ScreenType screen;
    ScreenType defScreen;
} Screen;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);

Screen *screenGet(void);

void screenSetMode(ScreenType value);

void screenToClear(void);

void screenShow(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H
