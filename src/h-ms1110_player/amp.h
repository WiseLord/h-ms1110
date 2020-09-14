#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "audio/audiodefs.h"
#include "sync.h"

#define FLAG_EXIT           0
#define FLAG_ENTER          1
#define FLAG_SWITCH         2

// Mute and standby pins
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_0
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_1

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef struct {
    AmpStatus status;
    ScreenType screen;
    ScreenType defScreen;
    ScreenType prevScreen;
    bool clearScreen;

    InputType inType;


    uint8_t inputStatus;
    int8_t volume;

    Spectrum sp;
} Amp;

void ampInitMuteStby(void);

void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
