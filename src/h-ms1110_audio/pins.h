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

// Mute and standby pins
#define STBY_Port               GPIOB
#define STBY_Pin                LL_GPIO_PIN_0
#define MUTE_Port               GPIOB
#define MUTE_Pin                LL_GPIO_PIN_1

// Input
#define MATRIX_S1_Port          GPIOA
#define MATRIX_S1_Pin           LL_GPIO_PIN_13
#define MATRIX_S2_Port          GPIOA
#define MATRIX_S2_Pin           LL_GPIO_PIN_14
#define MATRIX_S3_Port          GPIOB
#define MATRIX_S3_Pin           LL_GPIO_PIN_3

#define MATRIX_K1_Port          GPIOB
#define MATRIX_K1_Pin           LL_GPIO_PIN_4
#define MATRIX_K2_Port          GPIOB
#define MATRIX_K2_Pin           LL_GPIO_PIN_5
#define MATRIX_K3_Port          GPIOB
#define MATRIX_K3_Pin           LL_GPIO_PIN_6
#define MATRIX_K4_Port          GPIOB
#define MATRIX_K4_Pin           LL_GPIO_PIN_7

// Remote control pins
#define RC_Port                 GPIOA
#define RC_Pin                  LL_GPIO_PIN_8
#define RC_ExtiLine             LL_EXTI_LINE_8
#define RC_AR_ExtiPort          LL_GPIO_AF_EXTI_PORTA
#define RC_AR_ExtiLine          LL_GPIO_AF_EXTI_LINE8

void pinsInit(void);

#ifdef __cplusplus
}
#endif

#endif // PINS_H
