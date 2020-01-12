#ifndef AMP_H
#define AMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "screen.h"

#define FLAG_EXIT       0
#define FLAG_ENTER      1
#define FLAG_SWITCH     2

typedef uint8_t ActionType;
enum {
    ACTION_NONE = 0,

    ACTION_BTN_SHORT,
    ACTION_BTN_LONG,
    ACTION_ENCODER,
    ACTION_REMOTE,

    ACTION_INIT,
    ACTION_INIT_HW,
    ACTION_INIT_RTC,
    ACTION_STANDBY,

    ACTION_DISP_EXPIRED,

    ACTION_END
};

typedef struct {
    ActionType type;
    ScrMode screen;

    int16_t value;
    int16_t timeout;
} Action;

typedef uint8_t AmpStatus;
enum {
    AMP_STATUS_STBY,
    AMP_STATUS_POWERED,
    AMP_STATUS_INIT,
    AMP_STATUS_HW_READY,
    AMP_STATUS_ACTIVE,

    AMP_STATUS_END
};

typedef struct {
    AmpStatus status;
    uint8_t inputStatus;
} Amp;

void ampActionGet(void);

void ampActionHandle(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
