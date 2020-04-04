#include "matrix.h"

#include "hwlibs.h"
#include "pins.h"

void inputMatrixInit(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = MATRIX_S1_Pin;
    LL_GPIO_Init(MATRIX_S1_Port, &initDef);
    initDef.Pin = MATRIX_S2_Pin;
    LL_GPIO_Init(MATRIX_S2_Port, &initDef);
    initDef.Pin = MATRIX_S3_Pin;
    LL_GPIO_Init(MATRIX_S3_Port, &initDef);

    initDef.Mode = LL_GPIO_MODE_FLOATING;

    initDef.Pin = MATRIX_K1_Pin;
    LL_GPIO_Init(MATRIX_K1_Port, &initDef);
    initDef.Pin = MATRIX_K2_Pin;
    LL_GPIO_Init(MATRIX_K2_Port, &initDef);
    initDef.Pin = MATRIX_K3_Pin;
    LL_GPIO_Init(MATRIX_K3_Port, &initDef);
    initDef.Pin = MATRIX_K4_Pin;
    LL_GPIO_Init(MATRIX_K4_Port, &initDef);

    CLR(MATRIX_S1);
    CLR(MATRIX_S2);
    CLR(MATRIX_S3);
}

uint16_t inputMatrixGet()
{
    static uint8_t row = 0;

    static uint16_t matrix;
    uint16_t cols = 0;

    if (READ(MATRIX_K1)) {
        cols |= 0x01;
    }
    if (READ(MATRIX_K2)) {
        cols |= 0x02;
    }
    if (READ(MATRIX_K3)) {
        cols |= 0x04;
    }
    if (READ(MATRIX_K4)) {
        cols |= 0x08;
    }

    switch (row) {
    case 0: // S1
        CLR(MATRIX_S1);
        SET(MATRIX_S2);
        break;
    case 1: // S2
        CLR(MATRIX_S2);
        SET(MATRIX_S3);
        break;
    case 2: // S3
        CLR(MATRIX_S3);
        SET(MATRIX_S1);
        break;
    }

    matrix &= ~(0x000F << (KEY_MATRIX_COLS * row));
    matrix |= (cols << (KEY_MATRIX_COLS * row));

    if (++row >= KEY_MATRIX_ROWS) {
        row = 0;
    }

    return matrix;
}
