#include "amp.h"

#include <stddef.h>
#include <string.h>

#include "debug.h"
#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "sync.h"
#include "timers.h"
#include "tr/labels.h"
#include "tuner/tuner.h"
#include "utils.h"

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetTimers(void);

static void ampActionSyncMaster(void);
static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampHandleSwd(void);
static void ampScreenShow(void);

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
    .defScreen = SCREEN_SPECTRUM,
    .inputStatus = 0x00,
    .volume = 0,
};

static Action action = {
    .type = ACTION_STANDBY,
    .screen = SCREEN_STANDBY,
    .value = FLAG_ENTER,
};

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionSetScreen(ScreenType screen, int16_t timeout)
{
    action.screen = screen;
    action.timeout = timeout;
}

void screenSetMode(ScreenType value)
{
    amp.screen = value;
}

static bool screenCheckClear(void)
{
    bool clear = false;

    if (amp.clearScreen) {
        clear = true;
        amp.clearScreen = false;
    } else {
        if (amp.screen != amp.prevScreen) {
            clear = true;
        }
    }

    amp.prevScreen = amp.screen;

    return clear;
}

static void actionDispExpired(ScreenType scrMode)
{
    ScreenType scrDef = SCREEN_SPECTRUM;

    rtcSetMode(RTC_NOEDIT);

    switch (scrMode) {
    case SCREEN_STANDBY:
        actionSetScreen(SCREEN_STANDBY, 0);
        break;
    default:
        actionSetScreen(scrDef, 0);
        break;
    }
}

static void ampReadSettings(void)
{
    tunerReadSettings();
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

    settingsStore(PARAM_DISPLAY_DEF, amp.defScreen);

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000);

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);
        break;
    case AMP_STATUS_HW_READY:

        amp.status = AMP_STATUS_ACTIVE;

        break;
    }
}


static void actionGetButtons(void)
{
    CmdBtn cmdBtn = inputGetBtnCmd();

    if (cmdBtn.btn) {
        Action action = {
            .type = cmdBtn.flags & BTN_FLAG_LONG_PRESS ? ACTION_TUNER_BTN_LONG : ACTION_TUNER_BTN_SHORT,
            .value = cmdBtn.btn,
        };
        syncSlaveSendAction(&action);
    }
}

static void actionGetEncoder(void)
{
    int8_t encVal = inputGetEncoder();

    if (encVal) {
        Action action = {
            .type = ACTION_TUNER_ENCODER,
            .value = encVal,
        };
        syncSlaveSendAction(&action);
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

    settingsInit();
    pinsInit();
    rtcInit();

    labelsInit();
    canvasInit();

    spInit();

    inputInit();

    syncSlaveInit(AMP_TUNER_ADDR);

    ampReadSettings();

    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger
    swTimInit();

    amp.status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    while (1) {
        ampHandleSwd();

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
    AmpSync sync;

    syncSlaveReceive(&sync);

    switch (sync.type) {
    case SYNC_ACTION:
        action = sync.action;
        break;
    case SYNC_TIME:
        rtcSetRaw(sync.time);
        break;
    }
}

void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        ScreenType scrMode = amp.screen;

        if (scrMode == SCREEN_STANDBY && rtcCheckAlarm()) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
        }
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }
}

static void ampActionRemap(void)
{
}

void ampActionHandle(void)
{
    ScreenType scrMode = amp.screen;

    switch (action.type) {
    case ACTION_INIT_HW:
        ampInitHw();
        break;
    case ACTION_STANDBY:
        if (action.value == FLAG_EXIT) {
            ampExitStby();
            actionSetScreen(SCREEN_TIME, 1000);
        } else {
            ampEnterStby();
            actionDispExpired(SCREEN_STANDBY);
        }
        break;
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired(scrMode);
        break;

    default:
        break;
    }

    screenSetMode(action.screen);

    if (action.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, action.timeout);
    } else if (action.timeout == 0) {
        swTimSet(SW_TIM_DISPLAY, SW_TIM_OFF);
    }

    action.type = ACTION_NONE;
    action.timeout = SW_TIM_OFF;
}

static void ampHandleSwd(void)
{
    static bool swd = false;

    if (SCREEN_STANDBY == amp.screen) {
        if (!swd) {
            LL_GPIO_AF_Remap_SWJ_NOJTAG();
            swd = true;
        }
    } else {
        if (swd) {
            LL_GPIO_AF_DisableRemap_SWJ();
            swd = false;
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

    switch (amp.screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, sp->mode, sp->peaks);
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

void TIM_SPECTRUM_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_SPECTRUM)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_SPECTRUM);

        // Callbacks
        spConvertADC();
    }
}
