#include "input.h"

#include "hwlibs.h"
#include "pins.h"
#include "settings.h"
#include "timers.h"

#define ADC_MAX         4095
#define ABS(x)          ((x) > 0 ? (x) : -(x))

#define BTN_THRESHOLD   100

static const uint32_t R_POT_H   = 4700;         // Pot pull-up resistor
static const uint32_t R_POT     = 100000;       // Pot resistance

static const uint32_t R_BTN_H   = 10000;    // Analog buttons pull-up resistor
static const uint32_t R_BTNS[ABTN_END] = {  // Analog buttons resistance
    100,
    15000,
    3900,
};

static const uint32_t analogInputs[AIN_END] = {
    AIN_POT_A_Channel,
    AIN_POT_B_Channel,
    AIN_BTN_ADC_Channel,
};

static InputCtx ctx;

static volatile int8_t encCnt;
static volatile CmdBtn cmdBuf;

static void inputAnalogInit(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);

    LL_GPIO_SetPinMode(AIN_BTN_Port, AIN_BTN_Pin, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(AIN_POT_A_Port, AIN_POT_A_Pin, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(AIN_POT_B_Port, AIN_POT_B_Pin, LL_GPIO_MODE_ANALOG);

    if (!LL_ADC_IsEnabled(ADC2)) {
        LL_ADC_SetDataAlignment(ADC2, LL_ADC_DATA_ALIGN_RIGHT);
        LL_ADC_SetSequencersScanMode(ADC2, LL_ADC_SEQ_SCAN_DISABLE);
        LL_ADC_REG_SetTriggerSource(ADC2, LL_ADC_REG_TRIG_SOFTWARE);
        LL_ADC_REG_SetContinuousMode(ADC2, LL_ADC_REG_CONV_SINGLE);
        LL_ADC_REG_SetSequencerLength(ADC2, LL_ADC_REG_SEQ_SCAN_DISABLE);

        for (uint8_t i = 0; i < AIN_END; i++) {
            LL_ADC_SetChannelSamplingTime(ADC2, analogInputs[i], LL_ADC_SAMPLINGTIME_71CYCLES_5);
        }

        LL_ADC_Enable(ADC2);
        while (!LL_ADC_IsEnabled(ADC2));

        LL_ADC_StartCalibration(ADC2);
        while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0);
    }

    ctx.zoneCnt = 15; // TODO: calculate from audio grid

    int16_t potMax = (ADC_MAX * R_POT / (R_POT + R_POT_H));
    int16_t zoneLen = potMax / ctx.zoneCnt;

    ctx.potData[AIN_POT_A] = zoneLen / 2;
    ctx.potData[AIN_POT_B] = zoneLen / 2;
}

static void inputAnalogConvert(void)
{
    static uint8_t chan = 0;

    // Read data from previous input
    int16_t adcData = (int16_t)LL_ADC_REG_ReadConversionData12(ADC2);
    ctx.adcData[chan] = adcData; // TODO: remove

    if (chan < AIN_POT_END) {
        int16_t potMax = (ADC_MAX * R_POT / (R_POT + R_POT_H));
        int16_t zoneLen = potMax / ctx.zoneCnt;

        // Consider "reverted" potentiomener
        adcData = potMax - adcData;

        // Filter data to nearest zone value
        int16_t filterWidth = zoneLen * 3 / 4;
        ctx.potData[chan] += (adcData - ctx.potData[chan]) / filterWidth * filterWidth;
    } else if (chan == AIN_BTN) {
        ctx.aBtn = ABTN_RELEASED;
        for (AnalogBtn i = 0; i < ABTN_END; i++) {
            int16_t btnDiff = (int16_t)(ADC_MAX * R_BTNS[i] / (R_BTNS[i] + R_BTN_H)) - adcData;
            if (ABS(btnDiff) < BTN_THRESHOLD) {
                ctx.aBtn = i;
                break;
            }
        }
    }

    // Change input
    if (++chan >= AIN_END) {
        chan = 0;
    }

    // Run new conversion
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, analogInputs[chan]);
    LL_ADC_REG_StartConversionSWStart(ADC2);
}

static uint16_t getAnalogButtons()
{
    if (ctx.aBtn >= 0) {
        return (uint8_t)(BTN_STBY << ctx.aBtn);
    }

    return BTN_NO;
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

    btnNow |= getAnalogButtons();
    btnNow |= readEncoder();

    // If encoder event has happened, inc/dec encoder counter
    if (ctx.encRes) {
        uint16_t encNow = btnNow & ENC_AB;
        btnNow &= ~ENC_AB;

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
                if (btnNow & (ENC_A | ENC_B)) {
                    btnCnt -= AUTOREPEAT;
                }
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
    inputAnalogInit();
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

        inputAnalogConvert();
        inputHandle();
    }
}

InputCtx *inputGetCtx()
{
    return &ctx;
}

int8_t inputGetPot(uint8_t chan)
{
    int16_t potMax = (ADC_MAX * R_POT / (R_POT + R_POT_H));
    return (int8_t)(ctx.potData[chan] * ctx.zoneCnt / potMax);
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
