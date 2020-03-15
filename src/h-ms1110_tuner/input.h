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

// Tuner buttons
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

typedef struct {
    int8_t encRes;
    uint16_t matrix;
} InputCtx;

void inputInit(void);

void inputSetEncRes(int8_t value);
int8_t inputGetEncRes(void);

int8_t getEncoder(void);
CmdBtn getBtnCmd(void);

InputCtx *inputGetCtx(void);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
