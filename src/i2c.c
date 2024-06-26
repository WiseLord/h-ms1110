#include "i2c.h"

#include <stddef.h>

#include "hwlibs.h"

#define I2C_TIMEOUT_MS      25

typedef struct {
    I2cRxFn rxCb;
    I2cTxFn txCb;
    uint8_t *txBuf;
    uint8_t *rxBuf;
    int16_t txIdx;
    int16_t rxIdx;
    int16_t bytes;
    uint8_t addr;
    uint8_t direction;
    uint8_t timeout;
} I2cContext;

#if I2C1_BUF_SIZE
static uint8_t i2c1Buf[I2C1_BUF_SIZE];
static I2cContext i2cCtx1 = {
    .txBuf = i2c1Buf,
};
#endif

#if I2C2_BUF_SIZE
static uint8_t i2c2Buf[I2C2_BUF_SIZE];
static I2cContext i2cCtx2 = {
    .txBuf = i2c2Buf,
};
#endif

static I2cContext *i2cGetCtx(void *i2c)
{
#if I2C1_BUF_SIZE
    if (i2c == I2C1) {
        return &i2cCtx1;
    }
#endif
#if I2C2_BUF_SIZE
    if (i2c == I2C2) {
        return &i2cCtx2;
    }
#endif
    return NULL;
}

static void i2cInitPins(I2C_TypeDef *I2Cx)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef *gpio = GPIOB;
    uint32_t sclPin =  LL_I2C1_SCL_PIN;
    uint32_t sdaPin =  LL_I2C1_SDA_PIN;

    if (I2Cx == I2C1) {
        if (sclPin == LL_GPIO_PIN_8 && sdaPin == LL_GPIO_PIN_9) {
#ifdef STM32F1
            LL_GPIO_AF_EnableRemap_I2C1();
#endif
        }
    } else if (I2Cx == I2C2) {
        sclPin = LL_GPIO_PIN_10;
        sdaPin = LL_GPIO_PIN_11;
    }

    GPIO_InitStruct.Pin = sclPin | sdaPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
#endif

    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}

__attribute__((always_inline))
static inline void i2cEnableInterrupts(I2C_TypeDef *I2Cx)
{
#ifdef STM32F1
    // Enable Events interrupts (SB, ADDR, ADD10, STOPF, BTF)
    SET_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN);
    // Enable Error interrupts (BERR, ARLO, AF, OVR)
    SET_BIT(I2Cx->CR2, I2C_CR2_ITERREN);
    // Enable Buffer interrupts (RXNE, TXE)
    SET_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
#endif
}

__attribute__((always_inline))
static inline void i2cDisableInterrupts(I2C_TypeDef *I2Cx)
{
#ifdef STM32F1
    // Enable Events interrupts (SB, ADDR, ADD10, STOPF, BTF)
    CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN);
    // Enable Error interrupts (BERR, ARLO, AF, OVR)
    CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITERREN);
    // Enable Buffer interrupts (RXNE, TXE)
    CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
#endif
}

static bool i2cWait(I2cContext *ctx)
{
    if (LL_SYSTICK_IsActiveCounterFlag()) {
        if (ctx->timeout-- == 0) {
            // TODO: Handle errors
            return false;
        }
    }
    return true;
}

#ifdef STM32F1
static void i2cDoStop(I2C_TypeDef *I2Cx)
{
    I2cContext *ctx = i2cGetCtx(I2Cx);

    i2cDisableInterrupts(I2Cx);

    LL_I2C_GenerateStopCondition(I2Cx);
    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(I2Cx)) {
        if (i2cWait(ctx) == false) {
            return;
        }
    }
}
#endif

bool i2cInit(void *i2c, uint32_t ClockSpeed, uint8_t ownAddr)
{
    I2cContext *ctx = i2cGetCtx(i2c);

    if (ctx == NULL) {
        return false;
    }

    I2C_TypeDef *I2Cx = (I2C_TypeDef *)i2c;

    if (I2Cx == I2C1) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

        NVIC_SetPriority(I2C1_EV_IRQn, 0);
        NVIC_EnableIRQ(I2C1_EV_IRQn);

        NVIC_SetPriority(I2C1_ER_IRQn, 1);
        NVIC_EnableIRQ(I2C1_ER_IRQn);
    } else if (I2Cx == I2C2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

        NVIC_SetPriority(I2C2_EV_IRQn, 0);
        NVIC_EnableIRQ(I2C2_EV_IRQn);

        NVIC_SetPriority(I2C2_ER_IRQn, 1);
        NVIC_EnableIRQ(I2C2_ER_IRQn);
    }

    i2cInitPins(I2Cx);

#ifdef STM32F3
    LL_I2C_EnableAutoEndMode(i2c);
#endif
    LL_I2C_DeInit(I2Cx);

    LL_I2C_DisableOwnAddress2(I2Cx);
    LL_I2C_DisableGeneralCall(I2Cx);
    LL_I2C_EnableClockStretching(I2Cx);

    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
#ifdef STM32F1
    I2C_InitStruct.ClockSpeed = ClockSpeed;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
#endif
#ifdef STM32F3
    I2C_InitStruct.Timing = 0x2000090E; // TODO: calculate from ClockSpeed
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
#endif
    I2C_InitStruct.OwnAddress1 = ownAddr;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2Cx, &I2C_InitStruct);

#ifdef STM32F3
    LL_I2C_SetOwnAddress2(I2Cx, 0, LL_I2C_OWNADDRESS2_NOMASK);
#endif
    return true;
}

bool i2cDeInit(void *i2c)
{
    LL_I2C_Disable(i2c);
    LL_I2C_DeInit(i2c);

    return true;
}

bool i2cIsEnabled(void *i2c)
{
    return (bool)(LL_I2C_IsEnabled(i2c));
}

void i2cSetRxCb(void *i2c, I2cRxFn cb)
{
    I2cContext *ctx = i2cGetCtx(i2c);

    ctx->rxCb = cb;
}

void i2cSetTxCb(void *i2c, I2cTxFn cb)
{
    I2cContext *ctx = i2cGetCtx(i2c);

    ctx->txCb = cb;
}


void i2cBegin(void *i2c, uint8_t addr)
{
    I2cContext *ctx = i2cGetCtx(i2c);
    if (ctx == NULL) {
        return;
    }

    ctx->bytes = 0;
    ctx->addr = addr;
}

void i2cSend(void *i2c, uint8_t data)
{
    I2cContext *ctx = i2cGetCtx(i2c);
    if (ctx == NULL) {
        return;
    }

    ctx->txBuf[ctx->bytes++] = data;
}

bool i2cTransmit(void *i2c)
{
    I2cContext *ctx = i2cGetCtx(i2c);
    if (ctx == NULL) {
        return false;
    }

#ifdef STM32F1
    I2C_TypeDef *I2Cx = (I2C_TypeDef *)i2c;

    i2cEnableInterrupts(I2Cx);

    // Set the I2C direction to Transmission
    ctx->direction = I2C_WRITE;

    // Clear address last bit (write mode)
    ctx->addr &= ~I2C_OAR1_ADD0;

    // Generate a START or RESTART condition
    SET_BIT(I2Cx->CR1, I2C_CR1_START);

    // Wait for START condition sent
    ctx->timeout = I2C_TIMEOUT_MS;
    while (READ_BIT(I2Cx->CR1, I2C_CR1_START) == I2C_CR1_START) {
        if (i2cWait(ctx) == false) {
            i2cDisableInterrupts(I2Cx);
            return false;
        }
    }

    // Wait until BUSY flag is reset: a STOP has been generated
    ctx->timeout = I2C_TIMEOUT_MS;
    while (READ_BIT(I2Cx->SR2, I2C_SR2_BUSY) == I2C_SR2_BUSY) {
        if (i2cWait(ctx) == false) {
            i2cDoStop(i2c);
            return false;
        }
    }
#endif

#ifdef STM32F3
    LL_I2C_HandleTransfer(i2c, ctx->addr, LL_I2C_ADDRSLAVE_7BIT, ctx->bytes,
                          LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    uint8_t *pBuf    = (uint8_t *)ctx->txBuf;

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (LL_I2C_IsActiveFlag_TXIS(i2c)) {
            LL_I2C_TransmitData8(i2c, (*pBuf++));
            ctx->timeout = I2C_TIMEOUT_MS;
        }
        if (i2cWait(ctx) == false) {
            LL_I2C_ClearFlag_STOP(i2c);
            return false;
        }
    }

    LL_I2C_ClearFlag_STOP(i2c);
#endif

    return true;
}

bool i2cReceive(void *i2c, uint8_t *rxBuf, int16_t bytes)
{
    I2cContext *ctx = i2cGetCtx(i2c);
    if (ctx == NULL) {
        return false;
    }

    ctx->rxBuf = rxBuf;
    ctx->bytes = bytes;

#ifdef STM32F1
    I2C_TypeDef *I2Cx = (I2C_TypeDef *)i2c;

    i2cEnableInterrupts(I2Cx);

    // Set the I2C direction to Reception
    ctx->direction = I2C_READ;

    // Set address last bit (read mode)
    ctx->addr |= I2C_OAR1_ADD0;

    // Generate a START or RESTART condition
    SET_BIT(I2Cx->CR1, I2C_CR1_START);

    // Wait for START condition sent
    ctx->timeout = I2C_TIMEOUT_MS;
    while (READ_BIT(I2Cx->CR1, I2C_CR1_START) == I2C_CR1_START) {
        if (i2cWait(ctx) == false) {
            i2cDisableInterrupts(I2Cx);
            return false;
        }
    }

    // Wait until BUSY flag is reset: a STOP has been generated
    ctx->timeout = I2C_TIMEOUT_MS;
    while (READ_BIT(I2Cx->SR2, I2C_SR2_BUSY) == I2C_SR2_BUSY) {
        if (i2cWait(ctx) == false) {
            i2cDoStop(i2c);
            return false;
        }
    }
    // Prepare the generation of a ACKnowledge to be ready for another reception
    SET_BIT(I2Cx->CR1, I2C_CR1_ACK);
#endif

#ifdef STM32F3
    LL_I2C_HandleTransfer(i2c, ctx->addr, LL_I2C_ADDRSLAVE_7BIT, bytes, LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);

    ctx->timeout = I2C_TIMEOUT_MS;
    while (!LL_I2C_IsActiveFlag_STOP(i2c)) {
        if (LL_I2C_IsActiveFlag_RXNE(i2c)) {
            *rxBuf++ = LL_I2C_ReceiveData8(i2c);
            ctx->timeout = I2C_TIMEOUT_MS;
        }
        if (i2cWait(ctx) == false)
            return false;
    }

    LL_I2C_ClearFlag_STOP(i2c);
#endif

    return true;
}


bool i2cSlaveTransmitReceive(void *i2c, uint8_t *rxBuf, int16_t bytes)
{
    I2cContext *ctx = i2cGetCtx(i2c);
    if (ctx == NULL) {
        return false;
    }

    ctx->rxBuf = rxBuf;
    ctx->bytes = bytes;

    i2cEnableInterrupts(i2c);

    return true;
}

#ifdef STM32F1
void I2C_EV_IRQHandler(I2C_TypeDef *I2Cx)
{
    I2cContext *ctx = i2cGetCtx(I2Cx);

    uint32_t SR1 = I2Cx->SR1;
    uint32_t SR2 = I2Cx->SR2;

    // If slave mode
    if (READ_BIT(SR2, I2C_SR2_MSL) != I2C_SR2_MSL) {
        // When Address matched flag (EV1)
        if (READ_BIT(SR1, I2C_SR1_ADDR) == I2C_SR1_ADDR) {
            // Initialize transmit/receive counters for next data transfer
            ctx->txIdx = 0;
            ctx->rxIdx = 0;
            SR1 = 0;
            SR2 = 0;
            if (ctx->txCb) {
                ctx->txCb(ctx->txIdx);
            }
        }
        // When Transmit data register empty flag (EV3)
        if (READ_BIT(SR1, I2C_SR1_TXE) == I2C_SR1_TXE) {
            // Write data in data register
            I2Cx->DR = ctx->txBuf[ctx->txIdx++];
            SR1 = 0;
            SR2 = 0;
        }
        // When Receive data register not empty flag (EV2)
        if (READ_BIT(SR1, I2C_SR1_RXNE) == I2C_SR1_RXNE) {
            // Read data from data register
            ctx->rxBuf[ctx->rxIdx++] = (uint8_t)I2Cx->DR;
            SR1 = 0;
            SR2 = 0;

        }
        // When Stop detection flag (EV4)
        if (READ_BIT(SR1, I2C_SR1_STOPF) == I2C_SR1_STOPF) {
            // Slave finishes to receive data - master STOP
            if (ctx->rxCb) {
                ctx->rxCb(ctx->rxIdx);
            }
            // Enable I2C peripheral
            SET_BIT(I2Cx->CR1, I2C_CR1_PE);
            SR1 = 0;
            SR2 = 0;
        }
    }
    // End slave mode

    // When Start condition is generated (EV5)
    if (READ_BIT(SR1, I2C_SR1_SB) == I2C_SR1_SB) {
        // Set the slave address for transmssion or for reception
        I2Cx->DR = ctx->addr;
        SR1 = 0;
        SR2 = 0;
    }

    // If master mode
    if (READ_BIT(SR2, I2C_SR2_MSL) == I2C_SR2_MSL) {
        // When Address is fully set flag (EV6)
        if (READ_BIT(SR1, I2C_SR1_ADDR) == I2C_SR1_ADDR) {
            // Write the first data in case the Master is Transmitter
            if (ctx->direction == I2C_WRITE) {
                // Initialize the Transmit counter
                ctx->txIdx = 0;
                // Write the first data in the data register
                I2Cx->DR = ctx->txBuf[ctx->txIdx++];
                // Decrement the number of bytes to be written
                ctx->bytes--;

                // With last byte sent, disable the Buffer interrupts
                if (ctx->bytes == 0) {
                    CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
                }
            } else {
                // Initialize Receive counter
                ctx->rxIdx = 0;
                // At this stage, ADDR is cleared because both SR1 and SR2 were read
                // EV6_1: Single byte reception
                if (ctx->bytes == 1) {
                    // Clear ACKnowledge
                    CLEAR_BIT(I2Cx->CR1, I2C_CR1_ACK);
                    // Program the STOP
                    SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
                }
            }
            SR1 = 0;
            SR2 = 0;
        }

        // Master transmits the remaining data
        // When Transmit data register empty but but transfer is not finished yet
        if (READ_BIT(SR1, I2C_SR1_BTF | I2C_SR1_TXE) == I2C_SR1_TXE) {
            /* If there is still data to write */
            if (ctx->bytes != 0) {
                /* Write the data in DR register */
                I2Cx->DR = ctx->txBuf[ctx->txIdx++];
                /* Decrment the number of data to be written */
                ctx->bytes--;
                /* If  no data remains to write, disable the BUF IT in order
                to not have again a TxE interrupt. */
                if (ctx->bytes == 0) {
                    /* Disable the BUF IT */
                    CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITBUFEN);
                }
            }
            SR1 = 0;
            SR2 = 0;
        }

        // If both BTF and TXE are set (EV8_2), program the STOP (tranferring last byte)
        if (READ_BIT(SR1, I2C_SR1_BTF | I2C_SR1_TXE) == (I2C_SR1_BTF | I2C_SR1_TXE)) {
            // Program the STOP
            SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
            // Disable EVT IT In order to not have a BTF IT again
            CLEAR_BIT(I2Cx->CR2, I2C_CR2_ITEVTEN);
            SR1 = 0;
            SR2 = 0;
        }
        // When Receive data register not empty flag
        if (READ_BIT(SR1, I2C_SR1_RXNE) == I2C_SR1_RXNE) {
            // Read data from data register
            ctx->rxBuf[ctx->rxIdx++] = (uint8_t)I2Cx->DR;
            // Decrement the number of bytes to be read
            ctx->bytes--;
            // EV7_1: Last byte reception
            if (ctx->bytes == 1) {
                // Clear ACKnowledge
                CLEAR_BIT(I2Cx->CR1, I2C_CR1_ACK);
                // Program the STOP
                SET_BIT(I2Cx->CR1, I2C_CR1_STOP);
            }
            SR1 = 0;
            SR2 = 0;
        }
    }
    // End master mode
}

void I2C1_EV_IRQHandler(void)
{
    I2C_EV_IRQHandler(I2C1);
}

void I2C2_EV_IRQHandler(void)
{
    I2C_EV_IRQHandler(I2C2);
}


void I2C_ER_IRQHandler(I2C_TypeDef *I2Cx)
{
    uint32_t SR1 = I2Cx->SR1;
    uint32_t SR2 = I2Cx->SR2;

    (void)SR2;

    // When an acknowledge failure is received after a byte transmission
    if (READ_BIT(SR1, I2C_SR1_AF) == I2C_SR1_AF) {
        CLEAR_BIT(I2Cx->SR1, I2C_SR1_AF);
        SR1 = 0;
    }

    // When arbitration lost
    if (READ_BIT(SR1, I2C_SR1_ARLO) == I2C_SR1_ARLO) {
        CLEAR_BIT(I2Cx->SR1, I2C_SR1_ARLO);
        SR1 = 0;
    }

    // When a misplaced Start or Stop condition is detected
    if (READ_BIT(SR1, I2C_SR1_BERR) == I2C_SR1_BERR) {
        CLEAR_BIT(I2Cx->SR1, I2C_SR1_BERR);
        SR1 = 0;
    }

    // When an overrun/underrun error occurs (Clock Stretching Disabled)
    if (READ_BIT(SR1, I2C_SR1_OVR) == I2C_SR1_OVR) {
        CLEAR_BIT(I2Cx->SR1, I2C_SR1_OVR);
        SR1 = 0;
    }
}

void I2C1_ER_IRQHandler(void)
{
    I2C_ER_IRQHandler(I2C1);
}

void I2C2_ER_IRQHandler(void)
{
    I2C_ER_IRQHandler(I2C2);
}
#endif
