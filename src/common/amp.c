#include "amp.h"

#include <stdbool.h>

#include "hwlibs.h"

void ampHandleSwd(ScreenType screen)
{
    static bool swd = false;

    if (SCREEN_STANDBY == screen) {
        if (!swd) {
            LL_GPIO_AF_Remap_SWJ_NOJTAG();
            swd = true;
        }
    } else {
        if (swd) {
            LL_GPIO_AF_DisableRemap_SWJ();
            swd = false;
        }
    }
}
