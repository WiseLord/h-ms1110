#include "amp.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input.h"
#include "mediakey.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "sync.h"
#include "timers.h"
#include "tr/labels.h"
#include "tuner/rds/demod.h"
#include "tuner/stations.h"
#include "tuner/tuner.h"
#include "tunersync.h"
#include "utils.h"

typedef struct {
    RdsParser rdsParser;
    ScreenType prevScreen;
    bool clearScreen;
    bool isSlave;
} AmpPriv;

static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);
static void actionRemapEncoder(int16_t encCnt);

static void ampActionSyncMaster(void);

static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampPollInput(void);
static void ampSyncTuner(void);

static void ampScreenShow(void);

static AmpPriv priv;
static Amp *amp;

static Action action = {
    .type = ACTION_NONE,
    .value = FLAG_ENTER,
};

static Screen screen = {
    .type = SCREEN_STANDBY,
    .timeout = SW_TIM_OFF,
};

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void screenSet(ScreenType type, int16_t timeout)
{
    screen.type = type;
    screen.timeout = timeout;
}

static void rdsParserCb(void)
{
    swTimSet(SW_TIM_RDS_HOLD, 1000);
}

static bool screenCheckClear(void)
{
    bool clear = false;

    if (priv.clearScreen) {
        clear = true;
        priv.clearScreen = false;
    } else {
        if (amp->screen != priv.prevScreen) {
            clear = true;
        }
    }

    priv.prevScreen = amp->screen;

    return clear;
}

static void actionDispExpired(void)
{
    ScreenType defScreen = SCREEN_SPECTRUM;

    switch (amp->inType) {
    case IN_TUNER:
        if (!priv.isSlave) {
            defScreen = SCREEN_TUNER;
        }
        break;
    }

    rtcSetMode(RTC_NOEDIT);

    switch (amp->screen) {
    case SCREEN_STANDBY:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(defScreen, 0);
        break;
    }
}

static void inputDisable(void)
{
    // TODO: only if it was tuner
    tunerSetMute(true);
    tunerSetPower(false);
}

static void inputEnable(void)
{
    Tuner *tuner = tunerGet();

    // TODO: only if it was tuner
    tunerSetPower(true);
    tunerSetFreq(tuner->status.freq);
    tunerSetVolume(tuner->par.volume);
    tunerSetMute(false);
}

static void ampReadSettings(void)
{
    tunerReadSettings(TUNER_IC_LC7213X);
}

void ampExitStby(void)
{
    ampReadSettings();

    amp->status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 200);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

    inputDisable();

    amp->status = AMP_STATUS_INACTIVE;
    swTimSet(SW_TIM_AMP_INIT, 1000);
}

void ampHandleStby(void)
{
    if (FLAG_SWITCH == action.value) {
        switch (amp->status) {
        case AMP_STATUS_STBY:
            action.value = FLAG_EXIT;
            break;
        case AMP_STATUS_ACTIVE:
            action.value = FLAG_ENTER;
            break;
        }
    }

    switch (action.value) {
    case FLAG_ENTER:
        ampEnterStby();
        screenSet(SCREEN_STANDBY, 0);
        break;
    case FLAG_EXIT:
        ampExitStby();
        screenSet(SCREEN_TIME, 1000);
        break;
    }
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp->status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000, 0x00);

        tunerInit();

        amp->status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();
        swTimSet(SW_TIM_INPUT_POLL, 200);

        swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
        amp->status = AMP_STATUS_ACTIVE;
        break;
    case AMP_STATUS_INACTIVE:
        swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
        amp->status = AMP_STATUS_STBY;
        break;
    }
}

static void actionGetTimers(void)
{
    if (swTimGet(SW_TIM_AMP_INIT) == 0) {
        actionSet(ACTION_INIT_HW, 0);
    } else if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    }
}

static void ampInitMuteStby(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

    GPIO_InitStruct.Pin = MUTE_Pin;
    LL_GPIO_Init(MUTE_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = STBY_Pin;
    LL_GPIO_Init(STBY_Port, &GPIO_InitStruct);
}

void ampInit(void)
{
    amp = ampGet();
    amp->inType = IN_TUNER;

    settingsInit();
    utilInitSysCounter();

    ampInitMuteStby();

    labelsInit();
    canvasInit();

    inputInit(BTN_NO, -4);

    spInit();

    syncSlaveInit(AMP_TUNER_ADDR);

    ampReadSettings();

    swTimInit();

    tunerSyncInit();
    rdsParserSetCb(rdsParserCb);

    amp->status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    amp = ampGet();

    while (1) {
        utilEnableSwd(SCREEN_STANDBY == amp->screen);

        ampActionSyncMaster();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampPollInput();
        ampSyncTuner();

        rdsDemodHandle();

        ampScreenShow();
    }
}

static void ampActionSyncMaster(void)
{
    uint8_t *syncData;
    uint8_t syncSize;

    if (!!READ(STBY)) {
        if (amp->status == AMP_STATUS_STBY) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
            return;
        }
    } else {
        if (amp->status == AMP_STATUS_ACTIVE) {
            actionSet(ACTION_STANDBY, FLAG_ENTER);
            return;
        }
    }

    syncSlaveReceive(&syncData, &syncSize);

    if (syncSize == 0) {
        return;
    }

    SyncType syncType = syncData[0];

    Spectrum *sp = spGet();

    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_TIME:
        rtcSetRaw(*(uint32_t *)&syncData[1]);
        break;
    case SYNC_SPECTRUM:
        *sp = *((Spectrum *)&syncData[1]);
        priv.clearScreen = true;
        break;
    case SYNC_IN_TYPE:
        amp->inType = *(InputType *)&syncData[1];
        actionSet(ACTION_DISP_EXPIRED, 0);
        break;
    case SYNC_REQUEST:
        priv.isSlave = true;
        tunerSyncInit();
        actionSet(ACTION_DISP_EXPIRED, 0);
        break;
    }
}

void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        action = ampGetButtons();
    }

    if (ACTION_NONE == action.type) {
        action = ampGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_TUNER_1:
        actionSet(ACTION_DIGIT, 1);
        break;
    case BTN_TUNER_2:
        actionSet(ACTION_DIGIT, 2);
        break;
    case BTN_TUNER_3:
        actionSet(ACTION_DIGIT, 3);
        break;
    case BTN_TUNER_4:
        actionSet(ACTION_DIGIT, 4);
        break;
    case BTN_TUNER_5:
        actionSet(ACTION_DIGIT, 5);
        break;
    case BTN_TUNER_6:
        actionSet(ACTION_DIGIT, 6);
        break;
    case BTN_TUNER_7:
        actionSet(ACTION_DIGIT, 7);
        break;
    case BTN_TUNER_8:
        actionSet(ACTION_DIGIT, 8);
        break;
    case BTN_TUNER_9:
        actionSet(ACTION_DIGIT, 9);
        break;
    case BTN_TUNER_MWFM:
        break;
    case BTN_TUNER_RDS:
        break;
    case BTN_TUNER_ENC:
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(int16_t button)
{
    switch (button) {
    case BTN_TUNER_1:
        actionSet(ACTION_DIGIT_HOLD, 1);
        break;
    case BTN_TUNER_2:
        actionSet(ACTION_DIGIT_HOLD, 2);
        break;
    case BTN_TUNER_3:
        actionSet(ACTION_DIGIT_HOLD, 3);
        break;
    case BTN_TUNER_4:
        actionSet(ACTION_DIGIT_HOLD, 4);
        break;
    case BTN_TUNER_5:
        actionSet(ACTION_DIGIT_HOLD, 5);
        break;
    case BTN_TUNER_6:
        actionSet(ACTION_DIGIT_HOLD, 6);
        break;
    case BTN_TUNER_7:
        actionSet(ACTION_DIGIT_HOLD, 7);
        break;
    case BTN_TUNER_8:
        actionSet(ACTION_DIGIT_HOLD, 8);
        break;
    case BTN_TUNER_9:
        actionSet(ACTION_DIGIT_HOLD, 9);
        break;
    case BTN_TUNER_MWFM:
        break;
    case BTN_TUNER_RDS:
        break;
    case BTN_TUNER_ENC:
         actionSet(ACTION_TUNER_STORE, 0);
        break;
    default:
        break;
    }
}

static void actionRemapEncoder(int16_t encCnt)
{
    if (amp->inType == IN_TUNER) {
        tunerStep(encCnt);
    } else {
        if (encCnt && priv.isSlave) {
            Action syncAction = {ACTION_MEDIA, encCnt > 0 ? MEDIAKEY_FFWD : MEDIAKEY_REWIND};
            syncSlaveSend(SYNC_ACTION, &syncAction, sizeof(Spectrum));
        }
        actionSet(ACTION_NONE, 0);
    }
}

static void ampActionRemap(void)
{
    switch (action.type) {
    case ACTION_BTN_SHORT:
        actionRemapBtnShort(action.value);
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong(action.value);
        break;
    case ACTION_ENCODER:
        actionRemapEncoder(action.value);
        break;
    }

    switch (action.type) {
    case ACTION_TUNER_STORE:
        if (amp->inType == IN_TUNER) {
            uint16_t freq = tunerGet()->status.freq;
            char *PS = rdsParserGet()->PS;
            stationStoreRemove(freq, PS);
        }
        break;
    case ACTION_DIGIT:
        if (amp->inType == IN_TUNER) {
            stationFavZap(action.value);
        }
        break;
    case ACTION_DIGIT_HOLD:
        if (amp->inType == IN_TUNER) {
            stationFavStoreRemove(action.value);
        }
        break;
    }
}

static void ampSendMediaKey(MediaKey key)
{
    switch (key) {
    case MEDIAKEY_PREV:
        stationSeek(-1);
        break;
    case MEDIAKEY_NEXT:
        stationSeek(+1);
        break;
    case MEDIAKEY_REWIND:
        tunerStep(-1);
        break;
    case MEDIAKEY_FFWD:
        tunerStep(+1);
        break;
    }
}

static void ampActionHandle(void)
{
    switch (action.type) {
    case ACTION_INIT_HW:
        ampInitHw();
        break;
    case ACTION_STANDBY:
        ampHandleStby();
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired();
        break;

    case ACTION_MEDIA:
        ampSendMediaKey((MediaKey)action.value);
        break;

    default:
        break;
    }

    if (action.type != ACTION_NONE) {
        amp->screen = screen.type;
    }

    if (screen.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, screen.timeout);
    } else if (screen.timeout == 0) {
        swTimSet(SW_TIM_DISPLAY, SW_TIM_OFF);
    }

    action.type = ACTION_NONE;
    screen.timeout = SW_TIM_OFF;
}

static void ampSyncTuner(void)
{
    if (syncTxIsBusy()) {
        return;
    }

    Tuner *tuner = tunerGet();
    TunerSync *sync = tunerSyncGet();
    int8_t stNum = stationGetNum(tuner->status.freq);

    if (sync->freq != tuner->status.freq) {
        sync->freq = tuner->status.freq;
        syncSlaveSend(SYNC_TUNER_FREQ, &sync->freq, sizeof(uint16_t));
        return;
    }

    if (sync->stNum != stNum) {
        sync->stNum = stNum;
        syncSlaveSend(SYNC_TUNER_STNUM, &stNum, sizeof(int8_t));
        return;
    }

    if (sync->tFlags != tuner->status.flags) {
        sync->tFlags = tuner->status.flags;
        syncSlaveSend(SYNC_TUNER_FLAGS, &sync->tFlags, sizeof(TunerFlag));
        return;
    }

    uint16_t favMask = stationFavGetMask(tuner->status.freq);
    if (sync->favMask != favMask) {
        sync->favMask = favMask;
        syncSlaveSend(SYNC_TUNER_FAVS, &sync->favMask, sizeof(uint16_t));
        return;
    }

    if (sync->band.fMin != tuner->par.fMin ||
        sync->band.fMax != tuner->par.fMax) {
        sync->band.fMin = tuner->par.fMin;
        sync->band.fMax = tuner->par.fMax;
        syncSlaveSend(SYNC_TUNER_BAND, &sync->band, sizeof(TunerSyncBand));
        return;
    }

    RdsParser *rdsParser = rdsParserGet();

    if (memcmp(rdsParser, &priv.rdsParser, sizeof(RdsParser))) {
        memcpy(&priv.rdsParser, rdsParser, sizeof(RdsParser));
        syncSlaveSend(SYNC_TUNER_RDS, rdsParser, sizeof (RdsParser));
        return;
    }
}

static void ampPollInput(void)
{
    if (amp->screen != SCREEN_STANDBY) {
        if (amp->inType == IN_TUNER) {
            if (swTimGet(SW_TIM_INPUT_POLL) == 0) {
                tunerUpdateStatus();
                swTimSet(SW_TIM_INPUT_POLL, 200);
            }
            if (swTimGet(SW_TIM_RDS_HOLD) == 0) {
                rdsParserReset();
                swTimSet(SW_TIM_RDS_HOLD, SW_TIM_OFF);
            }
        }
    }
}

void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    Spectrum *sp = spGet();
    SpMode spMode = sp->mode == SP_MODE_MIRROR ? SP_MODE_LEFT_MIRROR : SP_MODE_LEFT;

    switch (amp->screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, spMode, sp->peaks);
        break;
    case SCREEN_TIME:
        canvasShowDate(clear, true);
        break;
    case SCREEN_STANDBY:
        canvasShowDate(clear, false);
        break;
    case SCREEN_TUNER:
        canvasShowInputCommon(IN_TUNER, clear);
        canvasShowInputTuner(true);
        break;
    default:
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
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
