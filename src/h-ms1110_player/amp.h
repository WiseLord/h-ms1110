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

void ampInitMuteStby(void);

void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
