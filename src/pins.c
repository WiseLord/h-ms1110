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
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    // Enable clock for all GPIO peripherials

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = MUTE_Pin;
    LL_GPIO_Init(MUTE_Port, &initDef);
    SET(MUTE);

    initDef.Pin = STBY_Pin;
    LL_GPIO_Init(STBY_Port, &initDef);
    CLR(STBY);

    pinsInitDisplay();
}
