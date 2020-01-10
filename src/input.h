#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define R_H             4700
#define R_POT           100000

#define ADC_MAX         4095
#define POT_MAX         (ADC_MAX * R_POT / (R_POT + R_H))
#define POT_GAP         (ADC_MAX - POT_MAX)

typedef uint8_t AnalogInput;
enum {
    AIN_POT_A,
    AIN_POT_B,

    AIN_POT_END,

    AIN_BTN = AIN_POT_END,

    AIN_END
};

typedef struct {
    int16_t adcData[AIN_END];
    int16_t potData[AIN_POT_END];
    int8_t encRes;
    int8_t zoneCnt;
} InputCtx;

void inputInit(void);

InputCtx *inputGetCtx(void);

int8_t inputGetPot(uint8_t chan);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
