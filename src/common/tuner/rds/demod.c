#include "demod.h"

#include <stdbool.h>

#include "decoder.h"
#include "hwlibs.h"

static void rdsDemodInitPins(void)
{
#ifdef STM32F1
    LL_GPIO_AF_SetEXTISource(RDCL_AR_ExtiPort, RDCL_AR_ExtiLine);
    LL_GPIO_SetPinMode(RDCL_Port, RDCL_Pin, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(RDDA_Port, RDDA_Pin, LL_GPIO_MODE_FLOATING);
    LL_GPIO_SetPinMode(QUAL_Port, QUAL_Pin, LL_GPIO_MODE_FLOATING);
#endif

    LL_EXTI_DisableEvent_0_31(RDCL_ExtiLine);
    LL_EXTI_EnableIT_0_31(RDCL_ExtiLine);
    LL_EXTI_EnableFallingTrig_0_31(RDCL_ExtiLine);
}

static void rdsDemodIRQ(void)
{
    bool rdda = !!READ(RDDA);

    rdsDecoderPushBit(rdda);
}

void rdsDemodInit(void)
{
    rdsDemodInitPins();

    NVIC_SetPriority(EXTI2_IRQn, 0);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI_RDS_HANDLER()
{
    if (LL_EXTI_IsActiveFlag_0_31(RDCL_ExtiLine) != RESET) {
        // Clear RC line interrupt
        LL_EXTI_ClearFlag_0_31(RDCL_ExtiLine);

        // Callback
        rdsDemodIRQ();
    }
}

