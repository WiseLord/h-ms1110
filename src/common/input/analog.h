#ifndef INPUT_ANALOG_H
#define INPUT_ANALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AIN_BTN_Port            GPIOA
#define AIN_BTN_Pin             LL_GPIO_PIN_4
#define AIN_BTN_Channel         LL_ADC_CHANNEL_4
#define AIN_POT_A_Port          GPIOA
#define AIN_POT_A_Pin           LL_GPIO_PIN_2
#define AIN_POT_A_Channel       LL_ADC_CHANNEL_2
#define AIN_POT_B_Port          GPIOA
#define AIN_POT_B_Pin           LL_GPIO_PIN_3
#define AIN_POT_B_Channel       LL_ADC_CHANNEL_3

#define AIN_POT_ARRAY_LEN          8

typedef uint8_t AinChannel;
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
    uint16_t potRaw[AIN_POT_END][AIN_POT_ARRAY_LEN];
    uint16_t btnRaw;
    int8_t potZone[AIN_POT_END];
} InputAnalog;

void inputAnalogInit(void);
InputAnalog *inputAnalogGet(void);

void inputAnalogHandle(void);
AnalogBtn inputAnalogGetBtn(void);

uint16_t inputAnalogGetPot(AinChannel chan);
int8_t inputAnalogGetPotZone(AinChannel chan, uint8_t zoneCnt);

#ifdef __cplusplus
}
#endif

#endif // INPUT_ANALOG_H
