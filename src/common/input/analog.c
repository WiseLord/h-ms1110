#include "analog.h"

#include "audio/audio.h"
#include "input.h"
#include "pins.h"

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

static int16_t potZone;
static int16_t potData[AIN_POT_END];

void inputAnalogInit(void)
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
    potZone = R_POT_MAX / zoneCnt;

    potData[AIN_POT_A] = potZone / 2;
    potData[AIN_POT_B] = potZone / 2;
}

uint16_t inputAnalogGet(void)
{
    static uint8_t chan = 0;
    static AnalogBtn aBtn = ABTN_RELEASED;

    // Read data from previous input
    int16_t adcData = (int16_t)LL_ADC_REG_ReadConversionData12(ADC2);

    if (chan < AIN_POT_END) {
        int16_t potZone = potZone;

        // Consider "reverted" potentiomener
        adcData = R_POT_MAX - adcData;

        int16_t diff = adcData - potData[chan];

        if (diff >= 0) {
            potData[chan] += ABS(diff) / potZone * potZone;
        } else {
            potData[chan] -= ABS(diff) / potZone * potZone;
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

int8_t inputAnalogGetPots(uint8_t chan)
{
    const AudioGrid *grid = audioGet()->par.tune[AUDIO_TUNE_BASS].grid;
    const int16_t zoneCnt = grid->max - grid->min + 1;

    int8_t pot = (int8_t)(potData[chan] * zoneCnt / R_POT_MAX) + grid->min;

    if (pot < grid->min) {
        pot = grid->min;
    }
    if (pot > grid->max) {
        pot = grid->max;
    }

    return pot;
}
