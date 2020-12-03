#ifndef MEDIAKEY_H
#define MEDIAKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t MediaKey;
enum {
    MEDIAKEY_NONE       = 0,
    MEDIAKEY_PLAY,
    MEDIAKEY_PAUSE,
    MEDIAKEY_PLAY_PAUSE,
    MEDIAKEY_STOP,
    MEDIAKEY_PREVIOUS,
    MEDIAKEY_NEXT,
    MEDIAKEY_REPEAT,
    MEDIAKEY_EJECT,
    MEDIAKEY_REWIND,
    MEDIAKEY_FFWD,
};

#ifdef __cplusplus
}
#endif

#endif // MEDIAKEY_H
