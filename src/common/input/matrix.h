#ifndef INPUT_MATRIX_H
#define INPUT_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KEY_MATRIX_ROWS         3
#define KEY_MATRIX_COLS         4

void inputMatrixInit(void);
uint16_t inputMatrixGet(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_MATRIX_H
