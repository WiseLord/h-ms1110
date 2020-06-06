#ifndef INPUT_MATRIX_H
#define INPUT_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KEY_MATRIX_ROWS         3
#define KEY_MATRIX_COLS         4

// Input
#ifdef _PLAYER_PROTOTYPE
#define MATRIX_S1_Port          GPIOB
#define MATRIX_S1_Pin           LL_GPIO_PIN_0
#define MATRIX_S2_Port          GPIOB
#define MATRIX_S2_Pin           LL_GPIO_PIN_1
#else
#define MATRIX_S1_Port          GPIOA
#define MATRIX_S1_Pin           LL_GPIO_PIN_13
#define MATRIX_S2_Port          GPIOA
#define MATRIX_S2_Pin           LL_GPIO_PIN_14
#endif
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

void inputMatrixInit(void);
uint16_t inputMatrixGet(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_MATRIX_H
