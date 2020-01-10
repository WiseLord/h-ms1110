#include "input.h"

#include "hwlibs.h"
#include "pins.h"
#include "settings.h"
#include "timers.h"

static InputCtx ctx;

static const uint32_t analogInputs[] = {
    AIN_POT_A_Channel,
    AIN_POT_B_Channel,
    AIN_BTN_ADC_Channel,
};

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
    int16_t zoneLen = POT_MAX / ctx.zoneCnt;

    ctx.potData[AIN_POT_A] = POT_GAP + zoneLen;
    ctx.potData[AIN_POT_B] = POT_GAP + zoneLen;
}

static void inputAnalogConvert(void)
{
    static uint8_t chan = 0;

    // Read data from previous input
    int16_t adcData = (int16_t)LL_ADC_REG_ReadConversionData12(ADC2);
    ctx.adcData[chan] = adcData; // TODO: remove

    if (chan < AIN_POT_END) {
        int16_t zoneLen = POT_MAX / ctx.zoneCnt;
        adcData = ADC_MAX - adcData;
        // Filter data to nearest zone value
        ctx.potData[chan] += ((adcData - ctx.potData[chan]) / zoneLen) * zoneLen;
    }

    // Change input
    if (++chan >= AIN_END) {
        chan = 0;
    }

    // Run new conversion
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, analogInputs[chan]);
    LL_ADC_REG_StartConversionSWStart(ADC2);
}

void inputInit()
{
    inputAnalogInit();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling

    ctx.encRes = (int8_t)settingsGet(PARAM_SYSTEM_ENC_RES);
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

        inputAnalogConvert();
    }
}

InputCtx *inputGetCtx()
{
    return &ctx;
}

int8_t inputGetPot(uint8_t chan)
{
    return (ctx.potData[chan] - POT_GAP) * ctx.zoneCnt / POT_MAX;
}
