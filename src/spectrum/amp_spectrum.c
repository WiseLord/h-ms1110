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
} AmpPriv;

static void ampActionSyncMaster(void);

static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);

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
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    ampSetBrightness(AMP_BR_STBY);

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

static void actionGetTimers(void)
{
    if (swTimGet(SW_TIM_AMP_INIT) == 0) {
        actionSet(ACTION_INIT_HW, 0);
    } else if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    }
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

    syncSlaveInit(AMP_SPECTRUM_ADDR);

    ampReadSettings();

    swTimInit();

    amp->status = AMP_STATUS_STBY;
}

void ampSyncFromOthers(void)
{
    ampActionSyncMaster();
}

void ampSyncToOthers(void)
{

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

    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_TIME:
        rtcSetRaw(*(uint32_t *)&syncData[1]);
        break;
    case SYNC_IN_TYPE:
        amp->inType = *(InputType *)&syncData[1];
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

    syncSlaveSend(SYNC_SPECTRUM, sp, sizeof(Spectrum));
}

static void spPeaksChange()
{
    Spectrum *sp = spGet();

    sp->peaks = !sp->peaks;

    priv.clearScreen = true;

    settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);

    syncSlaveSend(SYNC_SPECTRUM, sp, sizeof(Spectrum));
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
        canvasShowSpectrum(clear, spMode, sp->peaks);
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

    glcdFbSync();
}
