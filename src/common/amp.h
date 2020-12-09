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

// Player matrix buttons
#define BTN_PLAYER_OPEN         0x0001
#define BTN_PLAYER_PLAYPAUSE    0x0002
#define BTN_PLAYER_STOP         0x0004
#define BTN_PLAYER_REWIND       0x0010
#define BTN_PLAYER_REPEATE      0x0020
#define BTN_PLAYER_FORWARD      0x0040
#define BTN_PLAYER_AUDIO        0x0008
#define BTN_PLAYER_SUBTITLE     0x0080

// Tuner matrix buttons
#define BTN_TUNER_1             0x0001
#define BTN_TUNER_2             0x0002
#define BTN_TUNER_3             0x0004
#define BTN_TUNER_4             0x0010
#define BTN_TUNER_5             0x0020
#define BTN_TUNER_6             0x0040
#define BTN_TUNER_7             0x0100
#define BTN_TUNER_8             0x0200
#define BTN_TUNER_9             0x0400
#define BTN_TUNER_MWFM          0x0008
#define BTN_TUNER_RDS           0x0080
#define BTN_TUNER_ENC           0x0800

// Spectrum matrix buttons
#define BTN_SPECTRUM_AUTO       0x0008
#define BTN_SPECTRUM_DEMO       0x0080
#define BTN_SPECTRUM_DISP_PREV  0x0400
#define BTN_SPECTRUM_DISP_NEXT  0x0200

// Amplifier analog buttons
#define BTN_AMP_STBY            0x1000
#define BTN_AMP_IN_PREV         0x2000
#define BTN_AMP_IN_NEXT         0x4000

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
    AMP_STATUS_STBY,        // Standby mode:    STBY=0, MUTE=1
    AMP_STATUS_POWERED,     // Power devices:   STBY=1, MUTE=1
    AMP_STATUS_HW_READY,    // HW is ready:     STBY=1, MUTE=1
    AMP_STATUS_ACTIVE,      // HW is inited:    STBY=1, MUTE=0
    AMP_STATUS_INACTIVE,    // Exiting active:  STBY=1, MUTE=1

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
} Amp;

void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
