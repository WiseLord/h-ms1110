#ifndef MPC_H
#define MPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "mediakey.h"

#define NAME_SIZE            128

typedef struct {
    char name[NAME_SIZE];
} Mpc;

void mpcInit(void);
Mpc *mpcGet(void);

void mpcSendMediaKey(MediaKey key);
void mpcGetData(void);

#ifdef __cplusplus
}
#endif

#endif // MPC_H
