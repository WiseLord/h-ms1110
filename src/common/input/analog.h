#ifndef INPUT_ANALOG_H
#define INPUT_ANALOG_H

#ifdef __cplusplus
extern "C" {
#endif

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

void inputAnalogInit(void);
uint16_t inputAnalogGet(void);

int8_t inputAnalogGetPots(uint8_t chan);

#ifdef __cplusplus
}
#endif

#endif // INPUT_ANALOG_H
