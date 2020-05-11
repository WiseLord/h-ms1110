#include "lc7213x.h"
#include "lc7213x_regs.h"

#include "hwlibs.h"
#include "tuner.h"
#include "utils.h"

// Software SPI
#define SPISW_DI_Port          GPIOA
#define SPISW_DI_Pin           LL_GPIO_PIN_7
#define SPISW_DO_Port          GPIOA
#define SPISW_DO_Pin           LL_GPIO_PIN_6
#define SPISW_CLK_Port         GPIOA
#define SPISW_CLK_Pin          LL_GPIO_PIN_5
#define SPISW_CE_Port          GPIOC
#define SPISW_CE_Pin           LL_GPIO_PIN_13

static void SPIswStrob()
{
    utiluDelay(1);
    SET(SPISW_CLK);
    utiluDelay(1);
    CLR(SPISW_CLK);
}

static void SPIswInitPins(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = SPISW_DI_Pin;
    LL_GPIO_Init(SPISW_DI_Port, &initDef);
    initDef.Pin = SPISW_CLK_Pin;
    LL_GPIO_Init(SPISW_CLK_Port, &initDef);
    initDef.Pin = SPISW_CE_Pin;
    LL_GPIO_Init(SPISW_CE_Port, &initDef);
}

void SPIswSendByte(uint8_t data)
{
    uint8_t i;
    uint8_t mask = 0x01;

    for (i = 0; i < 8; i++) {
        if (data & mask) {
            SET(SPISW_DI);
        } else {
            CLR(SPISW_DI);
        }

        SPIswStrob();

        data >>= 1;
    }
}

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi lc7213xApi = {
    .init = lc7213xInit,
    .setFreq = lc7213xSetFreq,
};

const TunerApi *lc7213xGetApi(void)
{
    return &lc7213xApi;
}

void lc7213xInit(TunerParam *param, TunerStatus *status)
{
    tPar = param;
    tStatus = status;

    tPar->fMin = 7600;
    tPar->fMax = 10800;
    tPar->fStep = 10;

    SPIswInitPins();

    lc7213xSetFreq(tStatus->freq);
}

void lc7213xSetFreq(uint16_t freq)
{
    uint16_t div;

    div = (freq + LC72131_IF) / LC72131_RF;

    SPIswSendByte(LC72131_IO_IN1);
    SET(SPISW_CE);
    SPIswSendByte(div & 0x00FF);
    SPIswSendByte((div & 0xFF00) >> 8);
    SPIswSendByte(0 | LC72131_IN1_DVS | LC72131_IN1_CTE | LC72131_IN1_R1 | LC72131_IN1_R0);
    CLR(SPISW_CE);

    SPIswSendByte(LC72131_IO_IN2);
    SET(SPISW_CE);
    SPIswSendByte(0 | LC72131_IN2_BO2 | LC72131_IN2_BO4);
    SPIswSendByte(0 | LC72131_IN2_BO5);
    SPIswSendByte(0);
    CLR(SPISW_CE);

    tunerGet()->status.freq = freq;
}
