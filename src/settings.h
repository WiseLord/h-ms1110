#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t Param;
enum {
    PARAM_NULL = 0,

    PARAM_SYSTEM_ENC_RES,

    PARAM_END
};

int16_t settingsGet(Param param);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
