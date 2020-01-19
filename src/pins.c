#include "pins.h"
#include <stdint.h>

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = DISP_CS_Pin;
    LL_GPIO_Init(DISP_CS_Port, &initDef);
    initDef.Pin = DISP_RS_Pin;
    LL_GPIO_Init(DISP_RS_Port, &initDef);

    initDef.Pin = DISP_RST_Pin;
    LL_GPIO_Init(DISP_RST_Port, &initDef);
}

void pinsInit(void)
{
    pinsInitDisplay();
}
