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

void mpcInit(void)
{
    ringBufInit(&rbuf, rbData, sizeof(rbData));

    usartInit(USART_MPC, 115200);
    usartSetRxIRQ(USART_MPC, true);
}

void mpcSyncRequest(void)
{
    mpcSendCmd("info");
}

Mpc *mpcGet(void)
{
    return &mpc;
}

void mpcLoadPlaylist(const char *name)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "load %s", name);
    mpcSendCmd(buf);
}

void mpcSendMediaKey(MediaKey key)
{
    switch (key) {
    case MEDIAKEY_PLAY:
        mpcSendCmd("play");
        break;
    case MEDIAKEY_PAUSE:
        mpcSendCmd("pause");
        break;
    case MEDIAKEY_STOP:
        mpcSendCmd("stop");
        break;
    case MEDIAKEY_PREV:
        mpcSendCmd("previous");
        break;
    case MEDIAKEY_NEXT:
        mpcSendCmd("next");
        break;
    case MEDIAKEY_REWIND:
        mpcSendCmd("rewind");
        break;
    case MEDIAKEY_FFWD:
        mpcSendCmd("ffwd");
        break;
    case MEDIAKEY_REPEAT:
        mpcSendCmd("repeat");
        break;
    case MEDIAKEY_RANDOM:
        mpcSendCmd("random");
        break;
    case MEDIAKEY_SINGLE:
        mpcSendCmd("single");
        break;
    case MEDIAKEY_CONSUME:
        mpcSendCmd("consume");
        break;
    case MEDIAKEY_EJECT:
        mpcSendCmd("load(\"Music\")");
        break;
    case MEDIAKEY_INJECT:
        mpcSendCmd("load(\"Radio\")");
        break;
    }
}

static void mpcUpdateMeta(const char *str)
{
    strncpy(mpc.meta, str, META_SIZE);
    utilTrimLineEnd(mpc.meta);
}

static void mpcParseCli(char *line)
{
    if (utilIsPrefix(line, "ELAPSED#: ")) {
        int elapsed;
        sscanf(line, "ELAPSED#: %d", &elapsed);
        mpc.elapsed = elapsed;
        mpc.flags |= MPC_FLAG_UPDATE_ELAPSED;
    } else if (utilIsPrefix(line, "DURATION#: ")) {
        int duration;
        sscanf(line, "DURATION#: %d", &duration);
        mpc.duration = duration;
        mpc.flags |= MPC_FLAG_UPDATE_DURATION;
        mpc.flags |= MPC_FLAG_UPDATE_META;
    } else if (utilIsPrefix(line, "META#: ")) {
        mpcUpdateMeta(line + strlen("META#: "));
        mpc.flags |= MPC_FLAG_UPDATE_META;
    } else if (utilIsPrefix(line, "PLAYING#")) {
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
        mpc.status |= MPC_PLAYING;
        mpc.status &= ~MPC_PAUSED;
    } else if (utilIsPrefix(line, "PAUSED#")) {
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
        mpc.status |= MPC_PAUSED;
    } else if (utilIsPrefix(line, "STOPPED#")) {
        mpc.flags |= (MPC_FLAG_UPDATE_STATUS | MPC_FLAG_UPDATE_ELAPSED);
        mpc.status &= ~(MPC_PLAYING | MPC_PAUSED);
    } else if (utilIsPrefix(line, "REPEAT#: ")) {
        int repeat;
        sscanf(line, "REPEAT#: %d", &repeat);
        repeat ? (mpc.status |= MPC_REPEAT) : (mpc.status &= ~MPC_REPEAT);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "RANDOM#: ")) {
        int random;
        sscanf(line, "RANDOM#: %d", &random);
        random ? (mpc.status |= MPC_RANDOM) : (mpc.status &= ~MPC_RANDOM);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "SINGLE#: ")) {
        int single;
        sscanf(line, "SINGLE#: %d", &single);
        single ? (mpc.status |= MPC_SINGLE) : (mpc.status &= ~MPC_SINGLE);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    } else if (utilIsPrefix(line, "CONSUME#: ")) {
        int consume;
        sscanf(line, "CONSUME#: %d", &consume);
        consume ? (mpc.status |= MPC_CONSUME) : (mpc.status &= ~MPC_CONSUME);
        mpc.flags |= MPC_FLAG_UPDATE_STATUS;
    }
}

static void mpcParseLine(char *line)
{
    mpcParseCli(line);
    if (utilIsPrefix(line, "##CLI.")) {
        mpcParseCli(line + strlen("##CLI."));
    }
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
