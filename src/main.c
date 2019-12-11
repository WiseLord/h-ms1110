#include "display/glcd.h"
#include "hwlibs.h"
#include "pins.h"
#include "usart.h"
#include "utils.h"

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0    ((uint32_t)0x00000007)
#define NVIC_PRIORITYGROUP_1    ((uint32_t)0x00000006)
#define NVIC_PRIORITYGROUP_2    ((uint32_t)0x00000005)
#define NVIC_PRIORITYGROUP_3    ((uint32_t)0x00000004)
#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#endif

void LL_Init(void)
{
#ifdef _STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // System interrupt init
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    // Configure HSE
    LL_RCC_HSE_Enable();
    // Wait till HSE is ready
    while (LL_RCC_HSE_IsReady() != 1);

    // Configure PLL
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    // Wait till PLL is ready
    while (LL_RCC_PLL_IsReady() != 1);

    // Configure system clock
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    // Wait till System clock is ready
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    // Configure APB1/APB2 clock
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // Configure SysTick
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
#ifdef _STM32F3
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
#endif
    // SysTick_IRQn interrupt configuration
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

void SysTick_Handler(void)
{
    static uint16_t tick = 0;

    if (++tick >= 200)
        tick = 0;

    switch (tick) {
    case 0:
        CLR(LED1);
        SET(LED3);
        break;
    case 100:
        CLR(LED3);
        SET(LED1);
        break;
    }
}

void printDispRegs(void)
{
#ifdef _DISP_READ_ENABLED

    const uint8_t num = 10;

    uint16_t args[num];

    for (uint16_t reg = 0x00; reg <= 0xFF; reg++) {
        dispdrvReset();
        dispdrvReadReg(reg, args, num);

        bool hasData = false;
        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                hasData = true;
                break;
            }
        }

        usartSendString(USART_DBG, utilMkStr("%02x: ", reg));

        if (!hasData) {
            usartSendString(USART_DBG, "\r");
            continue;
        }

        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                usartSendString(USART_DBG, utilMkStr("%04x ", args[i]));
            } else {
                usartSendString(USART_DBG, "---- ");
            }
        }
        usartSendString(USART_DBG, "\n\r");
    }
#else
    usartSendString(USART_DBG, "\rRead pin is disabled\r\n");
#endif
}

int main(void)
{
    // System
    LL_Init();
    SystemClock_Config();
    LL_SYSTICK_EnableIT();

    pinsInit();

    usartInit(USART_DBG, 115200);
    usartSendString(USART_DBG, "\rUsart init done\r\n");
//    printDispRegs();

    static Glcd *glcd;
    glcdInit(&glcd);
//    glcdRotate(LCD_ROTATE_180);

    // Graphics
    int16_t w = glcd->drv->width;
    int16_t h = glcd->drv->height;

    glcdDrawRect(0, 0, w, h, LCD_COLOR_BLACK);

    if (h >= 240) {
        glcdSetFont(&fontterminus32);
    } else if (h >= 176) {
        glcdSetFont(&fontterminus24);
    } else {
        glcdSetFont(&fontterminus16);
    }

    glcdSetFontColor(LCD_COLOR_RED);
    glcdSetXY(0, h / 16 * 2);
    glcdWriteString("Red line");
    glcd->drv->update();

    glcdSetFontColor(LCD_COLOR_LIME);
    glcdSetXY(0, h / 16 * 7);
    glcdWriteString("Green line");
    glcd->drv->update();

    glcdSetFontColor(LCD_COLOR_BLUE);
    glcdSetXY(0, h / 16 * 12);
    glcdWriteString("Blue line");
    glcd->drv->update();

    int16_t tw = w / 16;
    int16_t th = h / 4;

    glcdDrawRect(w / 2 + tw * 1, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_RED);
    glcdDrawRect(w / 2 + tw * 3, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_LIME);
    glcdDrawRect(w / 2 + tw * 5, h / 8 * 5, tw / 4 * 6, th, LCD_COLOR_BLUE);

    int16_t rx = w / 4 * 3;
    int16_t ry = h / 4 * 1;
    int16_t rr = ry - 8;
    glcdDrawRing(rx, ry, ry - 2, 3, LCD_COLOR_WHITE);

    while (1) {
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_RED);
        glcd->drv->update();
        LL_mDelay(500);
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_YELLOW);
        glcd->drv->update();
        LL_mDelay(500);
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_LIME);
        glcd->drv->update();
        LL_mDelay(500);
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_AQUA);
        glcd->drv->update();
        LL_mDelay(500);
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_BLUE);
        glcd->drv->update();
        LL_mDelay(500);
        glcdDrawCircle(rx, ry, rr, LCD_COLOR_MAGENTA);
        glcd->drv->update();
        LL_mDelay(500);
    }

    return 0;
}
