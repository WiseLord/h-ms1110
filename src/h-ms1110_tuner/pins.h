#ifndef PINS_H
#define PINS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "hwlibs.h"

// TFT parallel (8-bit) wiring
#define DISP_CS_Port            GPIOB
#define DISP_CS_Pin             LL_GPIO_PIN_12
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_14  // SPI DC
#define DISP_RST_Port           GPIOA
#define DISP_RST_Pin            LL_GPIO_PIN_15

#define ENCODER_Port            GPIOA
#define ENCODER_A_Pin           LL_GPIO_PIN_11
#define ENCODER_B_Pin           LL_GPIO_PIN_12

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

void pinsInit(void);

#ifdef __cplusplus
}
#endif

#endif // PINS_H
