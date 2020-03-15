#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define KEY_MATRIX_ROWS         3
#define KEY_MATRIX_COLS         4

#define BTN_NO                  0x0000

// Amplifier analog buttons
#define BTN_STBY                0x1000
#define BTN_IN_PREV             0x2000
#define BTN_IN_NEXT             0x4000

// Spectrum buttons
#define BTN_AUTO                0x0008
#define BTN_DEMO                0x0080
#define BTN_DISP_PREV           0x0400
#define BTN_DISP_NEXT           0x0200

#define ENC_NO                  0x0000
#define ENC_A                   0x2000
#define ENC_B                   0x4000
#define ENC_AB                  (ENC_A | ENC_B)

// Handling long press actions
#define SHORT_PRESS             20
#define LONG_PRESS              600
#define AUTOREPEAT              150

#define ENC_RES_MIN             -64
#define ENC_RES_MAX             64

#define BTN_FLAG_NO             0x0000
#define BTN_FLAG_LONG_PRESS     0x0001

typedef struct {
    uint16_t btn;
    uint16_t flags;
} CmdBtn;

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
    uint16_t matrix;
    AnalogBtn aBtn;
} InputCtx;

void inputInit(void);

void inputSetEncRes(int8_t value);
int8_t inputGetEncRes(void);

int8_t getEncoder(void);
CmdBtn getBtnCmd(void);

InputCtx *inputGetCtx(void);

int8_t inputGetPot(uint8_t chan);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
