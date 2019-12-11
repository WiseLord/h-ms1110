#include "input.h"

#include "hwlibs.h"

static int8_t encRes = 0;

static uint16_t adcData[AIN_END];
static const uint32_t adcChan[AIN_END] = {
    LL_ADC_CHANNEL_2,
    LL_ADC_CHANNEL_4,
    LL_ADC_CHANNEL_3,
};

static void inputInitAdc(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);

    if (!LL_ADC_IsEnabled(ADC2)) {
        LL_ADC_SetDataAlignment(ADC2, LL_ADC_DATA_ALIGN_RIGHT);

        LL_ADC_SetSequencersScanMode(ADC2, LL_ADC_SEQ_SCAN_DISABLE);

        LL_ADC_REG_SetTriggerSource(ADC2, LL_ADC_REG_TRIG_SOFTWARE);

        LL_ADC_REG_SetContinuousMode(ADC2, LL_ADC_REG_CONV_SINGLE);

        LL_ADC_REG_SetSequencerLength(ADC2, LL_ADC_REG_SEQ_SCAN_DISABLE);

        LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_71CYCLES_5);
        LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_3, LL_ADC_SAMPLINGTIME_71CYCLES_5);
        LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_71CYCLES_5);

        LL_ADC_Enable(ADC2);
        while (!LL_ADC_IsEnabled(ADC2));

        LL_ADC_StartCalibration(ADC2);
        while (LL_ADC_IsCalibrationOnGoing(ADC2) != 0);
    }
}

void inputInit()
{
    inputInitAdc();

    encRes = 2;//(int8_t)settingsRead(PARAM_SYSTEM_ENC_RES);
}

void inputConvertADC(void)
{
    if (LL_ADC_IsEnabled(ADC2)) {
        static uint8_t chan = 0;

        adcData[chan] = LL_ADC_REG_ReadConversionData12(ADC2);

        LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, adcChan[chan]);
        LL_ADC_REG_StartConversionSWStart(ADC2);

        if (++chan >= AIN_END) {
            chan = 0;
        }
    }
}

uint16_t *getAdcData()
{
    return adcData;
}
