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
    MPC_FLAG_UPDATE_META        = 0x0001,
    MPC_FLAG_UPDATE_ELAPSED     = 0x0002,
    MPC_FLAG_UPDATE_DURATION    = 0x0004,
    MPC_FLAG_UPDATE_STATUS      = 0x0008,
};

typedef uint8_t MpcStatus;
enum {
    MPC_STATUS_PLAYING,
    MPC_STATUS_PAUSED,
    MPC_STATUS_STOPPED,
};

typedef struct {
    char meta[META_SIZE];
    MpcFlags flags;
    int32_t elapsed;
    int32_t duration;
    MpcStatus status;
} Mpc;

void mpcInit(void);
Mpc *mpcGet(void);

void mpcSendMediaKey(MediaKey key);
void mpcGetData(void);

#ifdef __cplusplus
}
#endif

#endif // MPC_H
