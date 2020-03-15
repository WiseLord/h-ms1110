#include "debug.h"

#include "hwlibs.h"
#include "usart.h"

static void dbgPutString(const char *str)
{
    while (*str) {
        usartSendChar(USART_DBG, *str++);
    }
}

void dbgInit()
{
    usartInit(USART_DBG, 115200);
    usartSetRxIRQ(USART_DBG, true);

    usartSendChar(USART_DBG, '\r');
}

void dbg(const char *str)
{
    dbgPutString(str);
    dbgPutString("\r\n");
}

void USART_DBG_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_DBG) && LL_USART_IsEnabledIT_RXNE(USART_DBG)) {
        char data = LL_USART_ReceiveData8(USART_DBG);
        (void)data;
    } else {
        // Call Error function
    }
}
