#include "input.h"

#include "hwlibs.h"
#include "input/analog.h"
#include "input/matrix.h"
#include "settings.h"
#include "timers.h"

static Input input;

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

static void inputHandleButtons(uint16_t btnNow)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static uint16_t btnPrev = BTN_NO;

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                input.btn = btnPrev;
                input.flags |= BTN_FLAG_LONG_PRESS;
                if (btnNow & input.autoRepeat) {
                    btnCnt -= AUTOREPEAT;
                }
            }
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS - AUTOREPEAT)) {
            input.btn = btnPrev;
        }
        btnCnt = 0;
    }
    btnPrev = btnNow;
}

static void inputHandleEncoder(uint16_t encNow)
{
    // Previous state
    static uint16_t encPrev = ENC_NO;

    // If encoder event has happened, inc/dec encoder counter
    if (input.encRes) {
        if ((encPrev == ENC_NO && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_NO))
            input.encCnt++;
        if ((encPrev == ENC_NO && encNow == ENC_B) ||
            (encPrev == ENC_B && encNow == ENC_AB) ||
            (encPrev == ENC_AB && encNow == ENC_A) ||
            (encPrev == ENC_A && encNow == ENC_NO))
            input.encCnt--;
        encPrev = encNow;
    }
}

void inputInit(uint16_t autoRepeat)
{
    input.encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
    input.autoRepeat = autoRepeat;

    inputMatrixInit();
#ifdef _INPUT_ANALOG
    inputAnalogInit();
#endif // _INPUT_ANALOG
    inputEncoderInit();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling
}

Input *inputGet()
{
    return &input;
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

#ifdef _INPUT_ANALOG
        inputAnalogHandle();
#endif // _INPUT_ANALOG
        uint16_t encNow = readEncoder();
        uint16_t btnNow = inputMatrixGet();
#ifdef _INPUT_ANALOG
        AnalogBtn aBtn = inputAnalogGetBtn();
        if (aBtn >= 0) {
            btnNow |= (uint16_t)(BTN_ANALOG_START << aBtn);
        }
#endif // _INPUT_ANALOG

        inputHandleButtons(btnNow);
        inputHandleEncoder(encNow);
    }
}

int8_t inputGetEncoder(void)
{
    int8_t ret = 0;

    if (input.encRes) {
        ret = input.encCnt / input.encRes;
        input.encCnt -= (ret * input.encRes);
    } else {
        ret = input.encCnt;
        input.encCnt = 0;
    }

    return ret;
}

CmdBtn inputGetBtnCmd(void)
{
    CmdBtn ret =  { .btn = input.btn, .flags = input.flags };

    input.btn = BTN_NO;
    input.flags = BTN_FLAG_NO;

    return ret;
}
