#include "amp.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "swtimers.h"
#include "sync.h"
#include "timers.h"
#include "tr/labels.h"
#include "utils.h"

typedef struct {
    ScreenType prevScreen;
    bool clearScreen;
    bool spSyncNeeded;

    Action syncAction;

    uint8_t signalCnt;
} AmpPriv;

static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);

static void ampGetFromMaster(void);
static void ampSendToMaster(void);

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
    rtcSetMode(RTC_NOEDIT);

    switch (amp->screen) {
    case SCREEN_STANDBY:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(SCREEN_SPECTRUM, 0);
        break;
    }
}

static void inputDisable(void)
{
}

static void inputEnable(void)
{
}

static void ampReadSettings(void)
{
}

void ampExitStby(void)
{
    ampSetBrightness(AMP_BR_ACTIVE);

    ampReadSettings();

    amp->status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 200);
    swTimSet(SW_TIM_SILENCE, SW_TIM_ON);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    ampSetBrightness(AMP_BR_STBY);

    swTimSet(SW_TIM_STBY, SW_TIM_OFF);
    swTimSet(SW_TIM_SILENCE, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

    inputDisable();

    amp->status = AMP_STATUS_INACTIVE;
    spGet()->flags &= ~SP_FLAG_DEMO;
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

        amp->status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();

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
    } else if (swTimGet(SW_TIM_SYNC) == 0) {
        ampSetOffline(AMP_MODULE_PLAYER);
        actionSet(ACTION_DISP_EXPIRED, 0);
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

    settingsInit();
    utilInitSysCounter();

    ampInitMuteStby();

    labelsInit();
    canvasInit();

    ampSetBrightness(AMP_BR_STBY);

    inputInit(BTN_NO, 0);

    spInit();
    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger

    syncSlaveInit(AMP_SPECTRUM_ADDR);

    ampReadSettings();

    swTimInit();

    amp->status = AMP_STATUS_STBY;
    ampSetOffline(AMP_MODULE_PLAYER);
}

void ampSyncFromOthers(void)
{
    ampGetFromMaster();
}

void ampSyncToOthers(void)
{
    ampSendToMaster();
}

static void ampGetFromMaster(void)
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

    swTimSet(SW_TIM_SYNC, 1100);

    if (!ampIsOnline(AMP_MODULE_PLAYER)) {
        ampSetOnline(AMP_MODULE_PLAYER);
        actionSet(ACTION_DISP_EXPIRED, 0);
        return;
    }

    SyncType syncType = syncData[0];

    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_TIME:
        rtcSetRaw(*(uint32_t *)&syncData[1]);
        break;
    case SYNC_IN_TYPE:
        amp->inType = *(InputType *)&syncData[1];
        break;
    case SYNC_REQUEST:
        priv.spSyncNeeded = true;
        break;
    }
}

void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        action = ampGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_SPECTRUM_AUTO:
        actionSet(ACTION_SP_CHANGE_PEAKS, 0);
        break;
    case BTN_SPECTRUM_DEMO:
        actionSet(ACTION_SP_CHANGE_DEMO, FLAG_SWITCH);
        break;
    case BTN_SPECTRUM_DISP_PREV:
        actionSet(ACTION_SP_CHANGE_MODE, -1);
        break;
    case BTN_SPECTRUM_DISP_NEXT:
        actionSet(ACTION_SP_CHANGE_MODE, +1);
        break;
    }
}

static void actionRemapBtnLong(int16_t button)
{
    switch (button) {
    case BTN_SPECTRUM_AUTO:
        break;
    case BTN_SPECTRUM_DEMO:
        break;
    case BTN_SPECTRUM_DISP_PREV:
        break;
    case BTN_SPECTRUM_DISP_NEXT:
        break;
    }
}

void ampActionRemap(void)
{
    switch (action.type) {
    case ACTION_BTN_SHORT:
        actionRemapBtnShort(action.value);
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong(action.value);
        break;
    }
}

static void spModeChange(int16_t value)
{
    Spectrum *sp = spGet();
    if (value > 0) {
        if (++sp->mode > SP_MODE_STEREO_END) {
            sp->mode = SP_MODE_STEREO;
        }
    } else if (value < 0) {
        if (--sp->mode < SP_MODE_STEREO) {
            sp->mode = SP_MODE_STEREO_END;
        }
    }
    priv.clearScreen = true;
    settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
    priv.spSyncNeeded = true;
}

static void spPeaksChange()
{
    Spectrum *sp = spGet();
    sp->flags ^= SP_FLAG_PEAKS;
    priv.clearScreen = true;
    settingsStore(PARAM_SPECTRUM_PEAKS, (sp->flags & SP_FLAG_PEAKS) == SP_FLAG_PEAKS);
    priv.spSyncNeeded = true;
}

static void spDemoChange()
{
    Spectrum *sp = spGet();
    sp->flags ^= SP_FLAG_DEMO;
    priv.spSyncNeeded = true;
}

void ampActionHandle(void)
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

    case ACTION_SP_CHANGE_MODE:
        if (amp->screen == SCREEN_SPECTRUM) {
            spModeChange(action.value);
        }
        screenSet(SCREEN_SPECTRUM, 3000);
        break;

    case ACTION_SP_CHANGE_PEAKS:
        if (amp->screen == SCREEN_SPECTRUM) {
            spPeaksChange();
        }
        screenSet(SCREEN_SPECTRUM, 3000);
        break;

    case ACTION_SP_CHANGE_DEMO:
        if (action.value == FLAG_SWITCH) {
            spDemoChange();
        } else if (action.value == FLAG_ENTER) {
            spGet()->flags |= SP_FLAG_DEMO;
        } else if (action.value == FLAG_EXIT) {
            spGet()->flags &= ~SP_FLAG_DEMO;
        }
        priv.clearScreen = true;
        screenSet(SCREEN_SPECTRUM, 3000);
        break;

    default:
        break;
    }

    if (amp->status == AMP_STATUS_ACTIVE) {
        if (swTimGet(SW_TIM_SILENCE) == 0) {
            // Reset silence timer on signal
            if (spCheckSignal()) {
                priv.syncAction.type = ACTION_NO_SILENCE;
                swTimSet(SW_TIM_SILENCE, 1000);
            } else {
                swTimSet(SW_TIM_SILENCE, 100);
            }
        }
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

static void ampSendToMaster(void)
{
    if (!ampIsOnline(AMP_MODULE_PLAYER)) {
        syncTxResetBusy();
    }

    if (syncTxIsBusy()) {
        return;
    }

    if (priv.syncAction.type != SYNC_NONE) {
        syncSlaveSend(SYNC_ACTION, &priv.syncAction, sizeof(Action));
        priv.syncAction.type = SYNC_NONE;
        return;
    }

    if (priv.spSyncNeeded) {
        priv.spSyncNeeded = false;
        syncSlaveSend(SYNC_SPECTRUM, spGet(), sizeof(Spectrum));
        return;
    }
}

void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    Spectrum *sp = spGet();
    SpMode spMode = (sp->mode == SP_MODE_STEREO || sp->mode == SP_MODE_MIRROR) ?
                    SP_MODE_RIGHT_MIRROR : SP_MODE_RIGHT;

    switch (amp->screen) {
    case SCREEN_SPECTRUM:
        if ((sp->flags & SP_FLAG_DEMO) == SP_FLAG_DEMO) {
            canvasShowStars(clear, -40);
        } else {
            canvasShowSpectrum(clear, spMode, (sp->flags & SP_FLAG_PEAKS) == SP_FLAG_PEAKS);
        }
        break;
    case SCREEN_TIME:
        canvasShowDateTime(clear, DT_MODE_DATE);
        break;
    case SCREEN_STANDBY:
        canvasShowDateTime(clear, DT_MODE_DATE);
        break;
    default:
        break;
    }

    canvasDebugFPS();

    glcdSync();
}
