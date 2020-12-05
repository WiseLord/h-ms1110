#ifndef MPC_H
#define MPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "mediakey.h"

#define META_SIZE            128

typedef uint32_t MpcFlags;
enum {
    MPC_FLAG_UPDATE_META        = 0x01,
    MPC_FLAG_UPDATE_ELAPSED     = 0x02,
    MPC_FLAG_UPDATE_DURATION    = 0x04,
};

typedef struct {
    char meta[META_SIZE];
    MpcFlags flags;
    int32_t elapsed;
    int32_t duration;
} Mpc;

void mpcInit(void);
Mpc *mpcGet(void);

void mpcSendMediaKey(MediaKey key);
void mpcGetData(void);

#ifdef __cplusplus
}
#endif

#endif // MPC_H
