#include "input.h"

#include "hwlibs.h"
#include "pins.h"
#include "settings.h"
#include "timers.h"

static int8_t encRes = 0;

static const uint32_t analogInputs[] = {
    AIN_BTN_ADC_Channel,
    AIN_POT_A_Channel,
    AIN_POT_B_Channel,
};

static uint16_t adcData[AIN_END];

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
}

static void inputAnalogConvert(void)
{
    if (LL_ADC_IsEnabled(ADC2)) {
        static uint8_t chan = 0;

        // Read data from previous input
        adcData[chan] = LL_ADC_REG_ReadConversionData12(ADC2);

        // Change input
        if (++chan >= AIN_END) {
            chan = 0;
        }

        // Run new conversion
        LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, analogInputs[chan]);
        LL_ADC_REG_StartConversionSWStart(ADC2);
    }
}

void inputInit()
{
    inputAnalogInit();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling

    encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

        inputAnalogConvert();
    }
}

uint16_t *inputAnalogGetData()
{
    return adcData;
}
