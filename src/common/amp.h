#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "sync.h"

#define FLAG_EXIT           0
#define FLAG_ENTER          1
#define FLAG_SWITCH         2

// Mute and standby pins
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_0
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_1

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

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef int8_t InputType;
enum {
    IN_NULL = -1,

    IN_TUNER = 0,
    IN_MPD,
    IN_AUX1,
    IN_AUX2,

    IN_END
};

typedef struct {
    ScreenType type;
    int16_t timeout;
} Screen;

typedef struct {
    AmpStatus status;
    ScreenType screen;
    InputType inType;
    Spectrum sp;
} Amp;

void ampHandleSwd(ScreenType screen);


void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
