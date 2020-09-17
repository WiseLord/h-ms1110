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

void ampInit(void);
void ampRun(void);

Amp *ampGet(void);

#ifdef __cplusplus
}
#endif

#endif // AMP_H
