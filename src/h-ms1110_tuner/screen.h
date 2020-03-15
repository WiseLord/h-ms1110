#ifndef SCREEN_H
#define SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t ScrMode;
enum {
    // Screens allowed to be default
    SCREEN_SPECTRUM = 0,
    SCREEN_TIME,

    SCREEN_STANDBY,

    SCREEN_MENU,

    SCREEN_END
};

typedef struct {
    ScrMode mode;
    ScrMode def;
} Screen;

void screenReadSettings(void);
void screenSaveSettings(void);

void screenInit(void);

Screen *screenGet(void);

void screenSetMode(ScrMode value);

void screenToClear(void);

void screenShow(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_H
