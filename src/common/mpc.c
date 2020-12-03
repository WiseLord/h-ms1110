#include "mpc.h"

#include <stdio.h>
#include <string.h>

#include "hwlibs.h"
#include "ringbuf.h"
#include "usart.h"
#include "utils.h"

#define RING_BUF_SIZE 128

Mpc mpc;

static RingBuf rbuf;
static char rbData[RING_BUF_SIZE];
static LineParse lp;

static void mpcSendCmd(const char *cmd)
{
    const char *prefix = "\ncli.";

    char buf[32];
    snprintf(buf, sizeof(buf), "%s%s\r\n", prefix, cmd);
    usartSendString(USART_MPC, buf);
}

void mpcInit()
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));

    usartInit(USART_MPC, 115200);
    usartSetRxIRQ(USART_MPC, true);

    usartSendChar(USART_MPC, '\r');
}

Mpc *mpcGet(void)
{
    return &mpc;
}

void mpcSendMediaKey(HidMediaKey key)
{
    switch (key) {
    case HIDMEDIAKEY_PREV_TRACK:
        mpcSendCmd("previous");
        break;
    case HIDMEDIAKEY_NEXT_TRACK:
        mpcSendCmd("next");
        break;
    case HIDMEDIAKEY_STOP:
        mpcSendCmd("stop");
        break;
    case HIDMEDIAKEY_PLAY:
        mpcSendCmd("play");
        break;
    case HIDMEDIAKEY_PAUSE:
        mpcSendCmd("pause");
        break;
    case HIDMEDIAKEY_REWIND:
        break;
    case HIDMEDIAKEY_FFWD:
        break;
    case HIDMEDIAKEY_MUTE:
        break;
    }
}

static void mpcUpdateName(const char *str)
{
    strncpy(mpc.name, str, NAME_SIZE);
    utilTrimLineEnd(mpc.name);
}

static void mpcParseCli(char *line)
{
    mpcUpdateName(line);
}

static void mpcParseLine(char *line)
{
    mpcParseCli(line);
//    if (utilIsPrefix(line, "##CLI.")) {
//        karadioParseCli(line + strlen("##CLI."));
//    }
}

void mpcGetData(void)
{
    uint16_t size = ringBufGetSize(&rbuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            mpcParseLine(lp.line);
        }
    }
}

void USART_MPC_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_DBG) && LL_USART_IsEnabledIT_RXNE(USART_DBG)) {
        char data = LL_USART_ReceiveData8(USART_DBG);
        ringBufPushChar(&rbuf, data);
    } else {
        // Call Error function
    }
}
