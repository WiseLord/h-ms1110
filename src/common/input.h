#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define BTN_NO                  0x0000

// Spectrum matrix buttons
#define BTN_AUTO                0x0008
#define BTN_DEMO                0x0080
#define BTN_DISP_PREV           0x0400
#define BTN_DISP_NEXT           0x0200

// Tuner matrix buttons
#define BTN_1                   0x0001
#define BTN_2                   0x0002
#define BTN_3                   0x0004
#define BTN_4                   0x0010
#define BTN_5                   0x0020
#define BTN_6                   0x0040
#define BTN_7                   0x0100
#define BTN_8                   0x0200
#define BTN_9                   0x0400
#define BTN_MWFM                0x0008
#define BTN_RDS                 0x0080
#define BTN_ENC                 0x0800

// Encoder
#define ENC_NO                  0x0000
#define ENC_A                   0x2000
#define ENC_B                   0x4000
#define ENC_AB                  (ENC_A | ENC_B)

// Amplifier analog buttons
#define BTN_STBY                0x1000
#define BTN_IN_PREV             0x2000
#define BTN_IN_NEXT             0x4000

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

#ifdef _INPUT_ANALOG
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
#endif // _INPUT_ANALOG

typedef struct {
    uint16_t btn;
    uint16_t flags;
    int8_t encCnt;
    int8_t encRes;
#ifdef _INPUT_ANALOG
    int16_t potZone;
    int16_t potData[AIN_POT_END];
#endif // _INPUT_ANALOG
} Input;

void inputInit(void);
Input *inputGet(void);

int8_t inputGetEncoder(void);
CmdBtn inputGetBtnCmd(void);
#ifdef _INPUT_ANALOG
int8_t inputGetPots(uint8_t chan);
#endif // _INPUT_ANALOG

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
