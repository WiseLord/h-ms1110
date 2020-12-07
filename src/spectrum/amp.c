#include "amp.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
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

static void actionGetButtons(void);
static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);

static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampScreenShow(void);

static AmpPriv ampPriv;

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
};

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

    if (ampPriv.clearScreen) {
        clear = true;
        ampPriv.clearScreen = false;
    } else {
        if (amp.screen != ampPriv.prevScreen) {
            clear = true;
        }
    }

    ampPriv.prevScreen = amp.screen;

    return clear;
}

static void actionDispExpired(void)
{
    rtcSetMode(RTC_NOEDIT);

    switch (amp.screen) {
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
    ampReadSettings();

    amp.status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 200);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

    inputDisable();

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000, 0x00);

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();
        amp.status = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 200);

        break;
    }
}


static void actionGetButtons(void)
{
    CmdBtn cmdBtn = inputGetBtnCmd();

    if (cmdBtn.btn) {
        if (cmdBtn.flags & BTN_FLAG_LONG_PRESS) {
            actionSet(ACTION_BTN_LONG, (int16_t)cmdBtn.btn);
        } else {
            actionSet(ACTION_BTN_SHORT, (int16_t)cmdBtn.btn);
        }
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

void ampInit(void)
{
    dbgInit();

    utilInitSysCounter();

    settingsInit();

    labelsInit();
    canvasInit();

    spInit();

    inputInit(BTN_NO, 0);

    syncSlaveInit(AMP_SPECTRUM_ADDR);

    ampReadSettings();

    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger
    swTimInit();

    amp.status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    while (1) {
        utilEnableSwd(SCREEN_STANDBY == amp.screen);

        ampActionSyncMaster();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampScreenShow();
    }
}

Amp *ampGet(void)
{
    return &amp;
}

static void ampActionSyncMaster(void)
{
    uint8_t *syncData;
    uint8_t syncSize;

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
        amp.inType = *(InputType *)&syncData[1];
        actionSet(ACTION_DISP_EXPIRED, 0);
        break;
    }
}

void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_SPECTRUM_AUTO:
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

static void ampActionRemap(void)
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
            sp->peaks = !sp->peaks;
        }
    } else if (value < 0) {
        if (--sp->mode < SP_MODE_STEREO) {
            sp->mode = SP_MODE_STEREO_END;
            sp->peaks = !sp->peaks;
        }
    }

    ampPriv.clearScreen = true;

    settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
    settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);

    syncSlaveSendSpectrum(sp);
}

static void ampActionHandle(void)
{
    switch (action.type) {
    case ACTION_INIT_HW:
        ampInitHw();
        break;
    case ACTION_STANDBY:
        if (action.value == FLAG_EXIT) {
            ampExitStby();
            screenSet(SCREEN_TIME, 1000);
        } else {
            ampEnterStby();
            screenSet(SCREEN_STANDBY, 0);
        }
        break;
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired();
        break;

    case ACTION_SP_CHANGE_MODE:
        if (amp.screen == SCREEN_SPECTRUM) {
            spModeChange(action.value);
        }
        screenSet(SCREEN_SPECTRUM, 3000);
        break;

    default:
        break;
    }

    if (action.type != ACTION_NONE) {
        amp.screen = screen.type;
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

    SpMode spMode = sp->mode == SP_MODE_ANTIMIRROR ? SP_MODE_RIGHT_MIRROR : SP_MODE_RIGHT;

    switch (amp.screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, spMode, sp->peaks);
        break;
    case SCREEN_TIME:
        canvasShowDate(clear, true);
        break;
    case SCREEN_STANDBY:
        canvasShowDate(clear, false);
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
