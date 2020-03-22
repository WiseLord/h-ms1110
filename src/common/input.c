#include "input.h"

#include "audio/audio.h"
#include "hwlibs.h"
#include "pins.h"
#include "settings.h"
#include "timers.h"

#ifdef _INPUT_ANALOG

#define ADC_MAX         4095
#define ABS(x)          ((x) > 0 ? (x) : -(x))

#define BTN_THRESHOLD   100

static const uint32_t R_POT_H   = 4700;         // Pot pull-up resistor
static const uint32_t R_POT     = 100000;       // Pot resistance

// TODO: calibration settings for potentiomenters
static const int32_t R_POT_TH   = 67;           // Experimental threshold
static const int32_t R_POT_MAX  = (((ADC_MAX) * R_POT / (R_POT + R_POT_H)) - R_POT_TH);

static const uint32_t R_BTN_H   = 10000;        // Analog buttons pull-up resistor
static const uint32_t R_BTNS[ABTN_END] = {      // Analog buttons resistance
    100,
    15000,
    3900,
};

static const uint32_t analogInputs[AIN_END] = {
    AIN_POT_A_Channel,
    AIN_POT_B_Channel,
    AIN_BTN_ADC_Channel,
};

#endif // _INPUT_ANALOG

static Input input;

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

#ifdef _INPUT_ANALOG

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
            LL_ADC_SetChannelSamplingTime(ADC2, analogInputs[i], LL_ADC_SAMPLINGTIME_239CYCLES_5);
        }

        LL_ADC_Enable(ADC2);
        while (!LL_ADC_IsEnabled(ADC2));

        LL_ADC_StartCalibration(ADC2);
        while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0);
    }

    const AudioGrid *grid = audioGet()->par.tune[AUDIO_TUNE_BASS].grid;
    const int16_t zoneCnt = grid->max - grid->min + 1;
    input.potZone = R_POT_MAX / zoneCnt;

    input.potData[AIN_POT_A] = input.potZone / 2;
    input.potData[AIN_POT_B] = input.potZone / 2;
}

static uint16_t getAnalogInput()
{
    static uint8_t chan = 0;
    static AnalogBtn aBtn = ABTN_RELEASED;

    // Read data from previous input
    int16_t adcData = (int16_t)LL_ADC_REG_ReadConversionData12(ADC2);

    if (chan < AIN_POT_END) {
        int16_t potZone = input.potZone;

        // Consider "reverted" potentiomener
        adcData = R_POT_MAX - adcData;

        int16_t diff = adcData - input.potData[chan];

        if (diff >= 0) {
            input.potData[chan] += ABS(diff) / potZone * potZone;
        } else {
            input.potData[chan] -= ABS(diff) / potZone * potZone;
        }

    } else if (chan == AIN_BTN) {
        aBtn = ABTN_RELEASED;
        for (AnalogBtn i = 0; i < ABTN_END; i++) {
            int16_t btnDiff = (int16_t)(ADC_MAX * R_BTNS[i] / (R_BTNS[i] + R_BTN_H)) - adcData;
            if (ABS(btnDiff) < BTN_THRESHOLD) {
                aBtn = i;
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


    if (aBtn >= 0) {
        return (uint16_t)(BTN_STBY << aBtn);
    }

    return BTN_NO;
}

#endif // _INPUT_ANALOG

static uint16_t getMatrixButtons()
{
    static uint8_t row = 0;

    static uint16_t matrix;
    uint16_t cols = 0;

    if (READ(MATRIX_K1)) {
        cols |= 0x01;
    }
    if (READ(MATRIX_K2)) {
        cols |= 0x02;
    }
    if (READ(MATRIX_K3)) {
        cols |= 0x04;
    }
    if (READ(MATRIX_K4)) {
        cols |= 0x08;
    }

    switch (row) {
    case 0: // S1
        CLR(MATRIX_S1);
        SET(MATRIX_S2);
        break;
    case 1: // S2
        CLR(MATRIX_S2);
        SET(MATRIX_S3);
        break;
    case 2: // S3
        CLR(MATRIX_S3);
        SET(MATRIX_S1);
        break;
    }

    matrix &= ~(0x000F << (KEY_MATRIX_COLS * row));
    matrix |= (cols << (KEY_MATRIX_COLS * row));

    if (++row >= KEY_MATRIX_ROWS) {
        row = 0;
    }

    return matrix;
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

static void inputHandleButtons(void)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static uint16_t btnPrev = BTN_NO;

    uint16_t btnNow = BTN_NO;

    btnNow |= getMatrixButtons();
#ifdef _INPUT_ANALOG
    btnNow |= getAnalogInput();
#endif // _INPUT_ANALOG

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                input.btn = btnPrev;
                input.flags |= BTN_FLAG_LONG_PRESS;
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

static void inputHandleEncoder(void)
{
    // Previous state
    static uint16_t encPrev = ENC_NO;

    uint16_t encNow = readEncoder();

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

void inputInit()
{
    inputMatrixInit();
#ifdef _INPUT_ANALOG
    inputAnalogInit();
#endif // _INPUT_ANALOG
    inputEncoderInit();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling

    input.encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
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

        inputHandleButtons();
        inputHandleEncoder();
    }
}

#ifdef _INPUT_ANALOG

int8_t inputGetPots(uint8_t chan)
{
    const AudioGrid *grid = audioGet()->par.tune[AUDIO_TUNE_BASS].grid;
    const int16_t zoneCnt = grid->max - grid->min + 1;

    int8_t pot = (int8_t)(input.potData[chan] * zoneCnt / R_POT_MAX) + grid->min;

    if (pot < grid->min) {
        pot = grid->min;
    }
    if (pot > grid->max) {
        pot = grid->max;
    }

    return pot;
}

#endif // _INPUT_ANALOG

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
