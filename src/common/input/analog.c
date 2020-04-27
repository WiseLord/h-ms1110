#include "analog.h"

#include "audio/audio.h"
#include "hwlibs.h"
#include "input.h"

#define ADC_MAX         4095
#define ABS(x)          ((x) > 0 ? (x) : -(x))

#define BTN_THRESHOLD   100

static const int32_t potMin = 3850;             // Potentiomenter at minimum
static const int32_t potMax = 10;               // Potentiomenter at maximum

static const uint32_t R_BTN_H   = 10000;        // Analog buttons pull-up resistor
static const uint32_t R_BTNS[ABTN_END] = {      // Analog buttons resistance
    100,
    15000,
    3900,
};

static const uint32_t channels[AIN_END] = {
    AIN_POT_A_Channel,
    AIN_POT_B_Channel,
    AIN_BTN_Channel,
};

static InputAnalog ctx;

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
            LL_ADC_SetChannelSamplingTime(ADC2, channels[i], LL_ADC_SAMPLINGTIME_71CYCLES_5);
        }

        LL_ADC_Enable(ADC2);
        while (!LL_ADC_IsEnabled(ADC2));

        LL_ADC_StartCalibration(ADC2);
        while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0);
    }
}

InputAnalog *inputAnalogGet(void)
{
    return &ctx;
}

void inputAnalogHandle(void)
{
    static uint8_t chan;
    static uint8_t idx = 0;

    uint16_t raw = LL_ADC_REG_ReadConversionData12(ADC2);

    if (chan < AIN_POT_END) {
        ctx.potRaw[chan][idx] = raw;
    } else {
        ctx.btnRaw = raw;
    }

    // Change input
    if (++chan >= AIN_END) {
        chan = 0;
        if (++idx >= AIN_POT_ARRAY_LEN) {
            idx = 0;
        }
    }

    // Run new conversion
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, channels[chan]);
    LL_ADC_REG_StartConversionSWStart(ADC2);
}

uint16_t inputAnalogGetBtn(void)
{
    AnalogBtn aBtn = ABTN_RELEASED;

    for (AnalogBtn i = 0; i < ABTN_END; i++) {
        int16_t btnDiff = (int16_t)(ADC_MAX * R_BTNS[i] / (R_BTNS[i] + R_BTN_H)) - ctx.btnRaw;
        if (ABS(btnDiff) < BTN_THRESHOLD) {
            aBtn = i;
            break;
        }
    }

    if (aBtn >= 0) {
        return (uint16_t)(BTN_STBY << aBtn);
    }

    return BTN_NO;
}

uint16_t inputAnalogGetPot(AinChannel chan)
{
    uint16_t potAvg = 0;

    for (int16_t idx = 0; idx < AIN_POT_ARRAY_LEN; idx++) {
        potAvg += ctx.potRaw[chan][idx];
    }

    potAvg /= AIN_POT_ARRAY_LEN;

    return potAvg;
}

int8_t inputAnalogGetPotZone(AinChannel chan, uint8_t zoneCnt)
{
    int32_t potVal = inputAnalogGetPot(chan);

    int32_t zoneHalf = (potMax - potMin) / zoneCnt / 2;

    int32_t curVal = potMin + zoneHalf + ctx.potZone[chan] * (potMax - potMin) / zoneCnt;

    bool newZone = ABS(potVal - curVal) > ABS(zoneHalf * 3 / 2);

    if (newZone) {
        int32_t zone = (potVal - potMin) * zoneCnt / (potMax - potMin); // 11
        ctx.potZone[chan] = zone;
    }

    return ctx.potZone[chan];
}
