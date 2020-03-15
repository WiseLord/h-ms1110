#include "input.h"

#include "hwlibs.h"
#include "pins.h"
#include "settings.h"
#include "timers.h"

static InputCtx ctx;

static volatile int8_t encCnt;
static volatile CmdBtn cmdBuf;

static void inputMatrixInit(void)
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

static void inputMatrixRead(void)
{
    static uint8_t row = 0;

    uint16_t matrix = 0;

    if (READ(MATRIX_K1)) {
        matrix |= 0x01;
    }
    if (READ(MATRIX_K2)) {
        matrix |= 0x02;
    }
    if (READ(MATRIX_K3)) {
        matrix |= 0x04;
    }
    if (READ(MATRIX_K4)) {
        matrix |= 0x08;
    }

    switch (row) {
    case 0: // S1
        ctx.matrix &= ~0x000F;
        ctx.matrix |= (matrix << 0);
        CLR(MATRIX_S1);
        SET(MATRIX_S2);
        break;
    case 1: // S2
        ctx.matrix &= ~0x00F0;
        ctx.matrix |= (matrix << 4);
        CLR(MATRIX_S2);
        SET(MATRIX_S3);
        break;
    case 2: // S3
        ctx.matrix &= ~0x0F00;
        ctx.matrix |= (matrix << 8);
        CLR(MATRIX_S3);
        SET(MATRIX_S1);
        break;
    }

    if (++row >= KEY_MATRIX_ROWS) {
        row = 0;
    }
}

static uint16_t getMatrixButtons()
{
    return ctx.matrix;
}

static void inputEncoderInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStructf;

    GPIO_InitStructf.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStructf.Pull = LL_GPIO_PULL_UP;

    GPIO_InitStructf.Pin = ENCODER_A_Pin | ENCODER_B_Pin;

    LL_GPIO_Init(ENCODER_Port, &GPIO_InitStructf);
}

static uint16_t readEncoder(void)
{
    uint16_t enc = ENC_NO;

    uint32_t encPort = LL_GPIO_ReadInputPort(ENCODER_Port);

    if (!(encPort & (ENCODER_A_Pin >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)) {
        enc |= ENC_A;
    }
    if (!(encPort & (ENCODER_B_Pin >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)) {
        enc |= ENC_B;
    }

    return enc;
}

static void inputHandle(void)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static uint16_t btnPrev = BTN_NO;
    static uint16_t encPrev = ENC_NO;

    uint16_t btnNow = BTN_NO;
    uint16_t encNow = BTN_NO;

    btnNow |= getMatrixButtons();

    encNow |= readEncoder();

    // If encoder event has happened, inc/dec encoder counter
    if (ctx.encRes) {
        if ((encPrev == ENC_NO && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_NO))
            encCnt++;
        if ((encPrev == ENC_NO && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_NO))
            encCnt--;
        encPrev = encNow;
    }

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                cmdBuf.btn = btnPrev;
                cmdBuf.flags |= BTN_FLAG_LONG_PRESS;
            }
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS - AUTOREPEAT)) {
            cmdBuf.btn = btnPrev;
        }
        btnCnt = 0;
    }
    btnPrev = btnNow;
}

void inputInit()
{
    inputMatrixInit();
    inputEncoderInit();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling

    ctx.encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
}

void inputSetEncRes(int8_t value)
{
    ctx.encRes = value;
}

int8_t inputGetEncRes(void)
{
    return ctx.encRes;
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

        inputMatrixRead();
        inputHandle();
    }
}

InputCtx *inputGetCtx()
{
    return &ctx;
}

int8_t getEncoder(void)
{
    int8_t ret = 0;

    if (ctx.encRes) {
        ret = encCnt / ctx.encRes;
        encCnt -= (ret * ctx.encRes);
    } else {
        ret = encCnt;
        encCnt = 0;
    }

    return ret;
}

CmdBtn getBtnCmd(void)
{
    CmdBtn ret = cmdBuf;

    cmdBuf.btn = BTN_NO;
    cmdBuf.flags = BTN_FLAG_NO;

    return ret;
}
