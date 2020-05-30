#include "dispdrv.h"

#include <stdbool.h>

static volatile bool busBusy;
static volatile uint8_t busData;

#ifdef _DISP_FB

typedef struct {
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
    int16_t x;
    int16_t y;
} FbArea;

static FbArea fbArea;

__attribute__((always_inline))
static inline void fbSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    fbArea.xMin = x;
    fbArea.yMin = y;
    fbArea.xMax = x + w;
    fbArea.yMax = y + h;
    fbArea.x = x;
    fbArea.y = y;
}

__attribute__((always_inline))
static inline void fbSetPixel(int16_t x, int16_t y, color_t color)
{
    if (dispdrv.fbSetPixel) {
        dispdrv.fbSetPixel(x, y, color);
    }
}

__attribute__((always_inline))
static inline void fbShiftPos()
{
    if (++fbArea.y >= fbArea.yMax) {
        fbArea.y = fbArea.yMin;
        if (++fbArea.x >= fbArea.xMax) {
            fbArea.x = fbArea.xMin;
        }
    }
}

#endif // _DISP_FB

__attribute__((always_inline))
static inline void dispdrvSendByte(uint8_t data)
{
#ifdef _DISP_SPI
    spiSendByte(SPI_DISPLAY, data);
#else
#if defined(_DISP_16BIT)
    WRITE_BYTE(DISP_DATA_HI, 0x00);
    WRITE_BYTE(DISP_DATA_LO, data);
#elif defined(_DISP_8BIT)
    WRITE_BYTE(DISP_DATA, data);
#endif
    __asm volatile ("nop");
    CLR(DISP_WR);
    __asm volatile ("nop");
    SET(DISP_WR);
#endif
}

__attribute__((always_inline))
static inline uint8_t  dispdrvReadBus(void)
{
    uint8_t ret = 0;

#if defined(_DISP_16BIT)
    ret = READ_BYTE(DISP_DATA_LO);
#else
#ifdef DISP_DATA_Port
    ret = READ_BYTE(DISP_DATA);
#endif
#endif

    return ret;
}

__attribute__((always_inline))
static inline void dispdrvBusIn(void)
{
#if defined(_DISP_16BIT)
    WRITE_BYTE(DISP_DATA_HI, 0xFF);         // Set HIGH level on all data lines
    WRITE_BYTE(DISP_DATA_LO, 0xFF);
#ifdef STM32F1
    DISP_DATA_HI_Port->CRH = 0x88888888;
    DISP_DATA_LO_Port->CRL = 0x88888888;
#endif
#ifdef STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
#endif
#elif defined(_DISP_8BIT)
    WRITE_BYTE(DISP_DATA, 0xFF);            // Set HIGH level on all data lines

#if IS_GPIO_LO(DISP_DATA)
#ifdef STM32F1
    DISP_DATA_Port->CRL = 0x88888888;
#endif
#ifdef STM32F3
    DISP_DATA_Port->MODER &= 0xFFFF0000;
#endif
#elif IS_GPIO_HI(DISP_DATA)
#ifdef STM32F1
    DISP_DATA_Port->CRH = 0x88888888;
#endif
#ifdef STM32F3
    DISP_DATA_Port->MODER &= 0x0000FFFF;
#endif
#endif
#endif
    busBusy = false;
}

__attribute__((always_inline))
static inline void dispdrvBusOut(void)
{
    if (!busBusy) {
        busData = dispdrvReadBus();
        busBusy = true;
    }
#if defined(_DISP_16BIT)
#ifdef STM32F1
    DISP_DATA_HI_Port->CRH = 0x33333333;
    DISP_DATA_LO_Port->CRL = 0x33333333;
#endif
#ifdef STM32F3
    DISP_DATA_HI_Port->MODER &= 0x0000FFFF;
    DISP_DATA_HI_Port->MODER |= 0x55550000;
    DISP_DATA_LO_Port->MODER &= 0xFFFF0000;
    DISP_DATA_LO_Port->MODER |= 0x55550000;
#endif
#elif defined(_DISP_8BIT)
#if IS_GPIO_LO(DISP_DATA)
#ifdef STM32F1
    DISP_DATA_Port->CRL = 0x33333333;
#endif
#ifdef STM32F3
    DISP_DATA_Port->MODER &= 0xFFFF0000;
    DISP_DATA_Port->MODER |= 0x00005555;
#endif
#endif
#if IS_GPIO_HI(DISP_DATA)
#ifdef STM32F1
    DISP_DATA_Port->CRH = 0x33333333;
#endif
#ifdef STM32F3
    DISP_DATA_Port->MODER &= 0x0000FFFF;
    DISP_DATA_Port->MODER |= 0x55550000;
#endif
#endif
#endif
}

__attribute__((always_inline))
static inline void dispdrvSendWord(uint16_t data)
{
    uint8_t dataH = data >> 8;
    uint8_t dataL = data & 0xFF;
#ifdef _DISP_16BIT
    WRITE_BYTE(DISP_DATA_HI, dataH);
    WRITE_BYTE(DISP_DATA_LO, dataL);

    __asm volatile ("nop");
    CLR(DISP_WR);
    __asm volatile ("nop");
    SET(DISP_WR);
#else
#ifndef _DISP_SPI
    dispdrvBusOut();
#endif
    dispdrvSendByte(dataH);
    dispdrvSendByte(dataL);
#ifndef _DISP_SPI
    dispdrvBusIn();
#endif

#endif // _DISP_16BIT
}

#ifdef _COLOR_24BIT
__attribute__((always_inline))
static inline void dispdrvSendTriplet(uint16_t data)
{
    uint8_t dataH = (data & 0xF800) >> 8;
    uint8_t dataM = (data & 0x07E0) >> 5;
    uint8_t dataL = (data & 0x001F) << 3;

#ifndef _DISP_SPI
    dispdrvBusOut();
#endif

    dispdrvSendByte(dataH);
    dispdrvSendByte(dataM);
    dispdrvSendByte(dataL);

#ifndef _DISP_SPI
    dispdrvBusIn();
#endif
}
#endif

__attribute__((always_inline))
static inline void dispdrvSendColor(color_t data)
{
#ifdef _DISP_FB
    fbSetPixel(fbArea.x, fbArea.y, data);
    fbShiftPos();
#else
#ifdef _COLOR_24BIT
    dispdrvSendTriplet(color);
#else
    dispdrvSendWord(data);
#endif
#endif
}

void dispdrvReset(void)
{
#ifdef _DISP_SPI
    spiInit(SPI_DISPLAY, false);
#else
    SET(DISP_CS);
#ifdef _DISP_READ_ENABLED
    SET(DISP_RD);
#endif
    SET(DISP_WR);
#endif
    SET(DISP_RS);
#ifdef _DISP_RST_ENABLED
    CLR(DISP_RST);
    utilmDelay(50);
    SET(DISP_RST);
#endif
    utilmDelay(50);
}

static void dispdrvInitPins(void)
{
    LL_GPIO_InitTypeDef initDef = {0};

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

    initDef.Pin = DISP_CS_Pin;
    LL_GPIO_Init(DISP_CS_Port, &initDef);
    initDef.Pin = DISP_RS_Pin;
    LL_GPIO_Init(DISP_RS_Port, &initDef);

#ifdef _DISP_RST
    initDef.Pin = DISP_RST_Pin;
    LL_GPIO_Init(DISP_RST_Port, &initDef);
#endif

#ifndef _DISP_SPI
    initDef.Pin = DISP_WR_Pin;
    LL_GPIO_Init(DISP_WR_Port, &initDef);
#endif
#ifdef _DISP_READ_ENABLED
    initDef.Pin = DISP_RD_Pin;
    LL_GPIO_Init(DISP_RD_Port, &initDef);
#endif
}

void dispdrvInit(void)
{
    dispdrvInitPins();

    dispdrvReset();

    dispdrv.init();
}

uint8_t dispdrvGetBus(void)
{
    if (!busBusy) {
        busData = dispdrvReadBus();
    }

    return busData;
}

void dispdrvSendData8(uint8_t data)
{
#ifdef _DISP_8BIT
    dispdrvBusOut();
#endif
    dispdrvSendByte(data);
#ifdef _DISP_8BIT
    dispdrvBusIn();
#endif
}

void dispdrvSendData16(uint16_t data)
{
    dispdrvSendWord(data);
}

void dispdrvSelectReg8(uint8_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendData8(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void dispdrvWriteReg8(uint8_t reg, uint8_t data)
{
    dispdrvSelectReg8(reg);
    dispdrvSendData8(data);
}

void dispdrvSelectReg16(uint16_t reg)
{
    DISP_WAIT_BUSY();
    CLR(DISP_RS);
    dispdrvSendWord(reg);
    DISP_WAIT_BUSY();
    SET(DISP_RS);
}

void dispdrvWriteReg16(uint16_t reg, uint16_t data)
{
    dispdrvSelectReg16(reg);
    dispdrvSendWord(data);
}

__attribute__((always_inline))
static inline void dispdrvReadDelay(void)
{
    volatile uint32_t ticks = 50;
    while (ticks--);
}

__attribute__((always_inline))
static inline uint8_t dispdrvReadByte(void)
{
    uint8_t ret = 0;

#ifdef DISP_RD_Port
    CLR(DISP_RD);
    dispdrvReadDelay();
    ret = dispdrvReadBus();
    SET(DISP_RD);
#endif

    return ret;
}

uint16_t dispdrvReadData16(void)
{
    uint16_t ret = 0;

#ifndef _DISP_SPI
    dispdrvBusIn();
#endif

#if defined(_DISP_16BIT)
#ifdef DISP_RD_Port
    CLR(DISP_RD);
    dispdrvReadDelay();
    ret |= READ_BYTE(DISP_DATA_HI) << 8;
    ret |= READ_BYTE(DISP_DATA_LO);
    SET(DISP_RD);
#endif
#elif defined(_DISP_8BIT)
    ret |= dispdrvReadByte();
    ret <<= 8;
    ret |= dispdrvReadByte();
#endif

#ifndef _DISP_SPI
    dispdrvBusOut();
#endif

    return ret;
}

void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs)
{
    CLR(DISP_CS);

    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);

    for (uint8_t i = 0; i < nArgs; i++) {
        args[i] = dispdrvReadData16();
    }

    SET(DISP_CS);
}

__attribute__((always_inline))
static inline void dispdrvSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
#ifdef _DISP_FB
    fbSetWindow(x, y, w, h);
#else
    if (dispdrv.setWindow) {
        dispdrv.setWindow(x, y, w, h);
    }
#endif
}

void dispdrvDrawPixel(int16_t x, int16_t y, color_t color)
{
#ifdef _DISP_FB
    fbSetPixel(x, y, color);
#else
    CLR(DISP_CS);

    dispdrv.setWindow(x, y, 1, 1);

    dispdrvSendColor(color);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    dispdrvSetWindow(x, y, w, h);

    for (int32_t i = 0; i < w * h; i++) {
        dispdrvSendColor(color);
    }

#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    dispdrvSetWindow(x, y, w, h);

    for (int32_t i = 0; i < w; i++) {
        color_t *color = gr;
        for (int32_t j = 0; j < h; j++) {
            dispdrvSendColor(*color++);
        }
    }

#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}

void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, color_t color, color_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h)
{
#ifndef _DISP_FB
    CLR(DISP_CS);
#endif

    dispdrvSetWindow(x, y, w, h);

    for (int16_t i = 0; i < w; i++) {
        for (int16_t j = 0; j < h; j++) {
            uint8_t data = img->data[img->width * ((j + yOft) >> 3) + i + xOft];
            if (j < h) {
                dispdrvSendColor(data & (1 << ((j + yOft) & 0x7)) ? color : bgColor);
            }
        }
    }

#ifndef _DISP_FB
    DISP_WAIT_BUSY();
    SET(DISP_CS);
#endif
}
