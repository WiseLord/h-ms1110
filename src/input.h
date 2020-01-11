#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t AnalogInput;
enum {
    AIN_POT_A,
    AIN_POT_B,

    AIN_POT_END,

    AIN_BTN = AIN_POT_END,

    AIN_END
};

typedef int8_t AnalogBtn;
enum {
    ABTN_RELEASED = -1,

    ABTN_STBY,
    ABTN_IN_PREV,
    ABTN_IN_NEXT,

    ABTN_END
};

typedef struct {
    int16_t adcData[AIN_END];
    int16_t potData[AIN_POT_END];
    int8_t encRes;
    int8_t zoneCnt;
    AnalogBtn aBtn;
} InputCtx;

void inputInit(void);

InputCtx *inputGetCtx(void);

int8_t inputGetPot(uint8_t chan);
AnalogBtn inputGetAnalogBtn(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
