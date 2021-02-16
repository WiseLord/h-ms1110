#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define BTN_NO                  0x0000
#define BTN_ANALOG_START        0x1000

// Encoder
#define ENC_NO                  0x0000
#define ENC_A                   0x2000
#define ENC_B                   0x4000
#define ENC_AB                  (ENC_A | ENC_B)

// Handling long press actions
#define SHORT_PRESS             60
#define LONG_PRESS              800
#define AUTOREPEAT              400

#define ENC_RES_MIN             -64
#define ENC_RES_MAX             64

#define BTN_FLAG_NO             0x0000
#define BTN_FLAG_LONG_PRESS     0x0001

#ifdef _MODULE_PLAYER
#define ENCODER_Port            GPIOA
#define ENCODER_A_Pin           LL_GPIO_PIN_0
#define ENCODER_B_Pin           LL_GPIO_PIN_1
#else
#define ENCODER_Port            GPIOA
#define ENCODER_A_Pin           LL_GPIO_PIN_11
#define ENCODER_B_Pin           LL_GPIO_PIN_12
#endif

typedef struct {
    uint16_t btn;
    uint16_t flags;
} CmdBtn;

typedef struct {
    uint16_t btn;
    uint16_t flags;
    uint16_t autoRepeat;
    int8_t encCnt;
    int8_t encRes;
} Input;

void inputInit(uint16_t autoRepeat, int8_t encRes);
Input *inputGet(void);

int8_t inputGetEncoder(void);
CmdBtn inputGetBtnCmd(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
