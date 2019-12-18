#include "settings.h"

int16_t settingsGet(Param param)
{
    int16_t ret = 0;

    switch (param) {
    case PARAM_SYSTEM_ENC_RES:
        ret = 2;
        break;
    default:
        break;
    }

    return  ret;
}
