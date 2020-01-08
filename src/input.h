#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t AnalogInput;
enum {
    AIN_BTN = 0,
    AIN_POT_A,
    AIN_POT_B,

    AIN_END
};

void inputInit(void);

uint16_t *inputAnalogGetData(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
