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

static uint8_t in1Buf[3] = {0x00, 0x00, 0x00};
static uint8_t in2Buf[3] = {0x00, 0x00, 0x00};
static uint8_t outBuf[3] = {0x00, 0x00, 0x00};

static TunerParam *tPar;
static TunerStatus *tStatus;

static const TunerApi lc7213xApi = {
    .init = lc7213xInit,
    .setFreq = lc7213xSetFreq,

    .setMute = lc7213xSetMute,
};

static void updateIn1(void)
{
    SPIswSendByte(LC72131_IO_IN1);
    SET(SPISW_CE);
    for (uint8_t i = 0; i < sizeof(in1Buf); i++) {
        SPIswSendByte(in1Buf[i]);
    }
    CLR(SPISW_CE);
}

static void updateIn2(void)
{
    SPIswSendByte(LC72131_IO_IN2);
    SET(SPISW_CE);
    for (uint8_t i = 0; i < sizeof(in2Buf); i++) {
        SPIswSendByte(in2Buf[i]);
    }
    CLR(SPISW_CE);
}

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

    in1Buf[2] &= ~LC72131_IN1_XS;   // Use 4.5MHz crystal
    in1Buf[2] |= (LC72131_IN1_R1 | LC72131_IN1_R0); // Use 25kHz base

    in2Buf[0] &= ~LC72131_IN2_IOC1; // Use IO1 as input
    in2Buf[0] &= ~LC72131_IN2_IOC2; // Use IO2 as input
    in2Buf[0] &= ~LC72131_IN2_BO1;  // Set FM mode
    in2Buf[0] &= ~LC72131_IN2_BO2;  // Set mute
    in2Buf[1] &= ~LC72131_IN2_BO5;  // Set stereo mode

    updateIn2();
}

void lc7213xSetFreq(uint16_t freq)
{
    uint16_t div;

    div = (freq + LC72131_IF) / LC72131_RF;

    in1Buf[0] = div & 0x00FF;
    in1Buf[1] = (div & 0xFF00) >> 8;

    in1Buf[2] |= LC72131_IN1_DVS;   // Use FM input
    in1Buf[2] |= LC72131_IN1_CTE;   // Counter start

    updateIn1();

    tunerGet()->status.freq = freq;
}

void lc7213xSetMute(bool value)
{
    if (value) {
        in2Buf[0] &= ~LC72131_IN2_BO2;
    } else {
        in2Buf[0] |= LC72131_IN2_BO2;
    }
    updateIn2();
}
