#ifndef PINS_H
#define PINS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "hwlibs.h"

#define CONCAT(x,y)             x ## y

#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))

#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)

// TFT parallel (8-bit) wiring
#define DISP_CS_Port            GPIOB
#define DISP_CS_Pin             LL_GPIO_PIN_12
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_14  // SPI DC
#define DISP_RST_Port           GPIOA
#define DISP_RST_Pin            LL_GPIO_PIN_15

#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_0
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_1

#define AIN_BTN_Port            GPIOA
#define AIN_BTN_Pin             LL_GPIO_PIN_4
#define AIN_BTN_ADC_Channel     LL_ADC_CHANNEL_4
#define AIN_POT_A_Port          GPIOA
#define AIN_POT_A_Pin           LL_GPIO_PIN_2
#define AIN_POT_A_Channel       LL_ADC_CHANNEL_2
#define AIN_POT_B_Port          GPIOA
#define AIN_POT_B_Pin           LL_GPIO_PIN_3
#define AIN_POT_B_Channel       LL_ADC_CHANNEL_3

void pinsInit(void);

#ifdef __cplusplus
}
#endif

#endif // PINS_H
