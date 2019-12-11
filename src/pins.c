#include "pins.h"
#include <stdint.h>

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

#if defined(_DISP_16BIT)
    initDef.Pin = DISP_DATA_LO_Pin;
    LL_GPIO_Init(DISP_DATA_LO_Port, &initDef);
    initDef.Pin = DISP_DATA_HI_Pin;
    LL_GPIO_Init(DISP_DATA_HI_Port, &initDef);
#elif defined (_DISP_8BIT)
    initDef.Pin = DISP_DATA_Pin;
    LL_GPIO_Init(DISP_DATA_Port, &initDef);
#endif

    initDef.Pin = DISP_CS_Pin;
    LL_GPIO_Init(DISP_CS_Port, &initDef);
    initDef.Pin = DISP_RS_Pin;
    LL_GPIO_Init(DISP_RS_Port, &initDef);
    initDef.Pin = DISP_BCKL_Pin;
    LL_GPIO_Init(DISP_BCKL_Port, &initDef);
#ifdef _DISP_READ_ENABLED
    initDef.Pin = DISP_RD_Pin;
    LL_GPIO_Init(DISP_RD_Port, &initDef);
#endif
#ifdef _DISP_RST_ENABLED
    initDef.Pin = DISP_RST_Pin;
    LL_GPIO_Init(DISP_RST_Port, &initDef);
#endif
#ifndef _DISP_SPI
    initDef.Pin = DISP_WR_Pin;
    LL_GPIO_Init(DISP_WR_Port, &initDef);
#endif
}

void pinsInit(void)
{
#ifdef _STM32F1
    LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif

    // Enable clock for all GPIO peripherials
#ifdef _STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
#endif
#ifdef _STM32F3
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#endif

    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

    initDef.Pin = LED1_Pin;
    LL_GPIO_Init(LED1_Port, &initDef);
    initDef.Pin = LED3_Pin;
    LL_GPIO_Init(LED1_Port, &initDef);

    pinsInitDisplay();
}
