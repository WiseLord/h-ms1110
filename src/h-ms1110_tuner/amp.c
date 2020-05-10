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

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);
static void actionRemapEncoder(int16_t encCnt);

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

static void actionDispExpired(void)
{
    amp.defScreen = SCREEN_SPECTRUM;

    switch (amp.inType) {
    case IN_TUNER:
        amp.defScreen = SCREEN_TUNER;
        break;
    }

    rtcSetMode(RTC_NOEDIT);

    switch (amp.screen) {
    case SCREEN_STANDBY:
        actionSetScreen(SCREEN_STANDBY, 0);
        break;
    default:
        actionSetScreen(amp.defScreen, 0);
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

    inputDisable();

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000);

        tunerInit();

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();
        amp.status = AMP_STATUS_ACTIVE;

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
        actionSet(ACTION_ENCODER, encVal);

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

    utilInitSysCounter();

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

    Spectrum *sp = spGet();

    switch (sync.type) {
    case SYNC_ACTION:
        action = sync.action;
        break;
    case SYNC_TIME:
        rtcSetRaw(sync.time);
        break;
    case SYNC_SPECTRUM:
        *sp = sync.spectrum;
        settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
        settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);
        amp.clearScreen = true;
        break;
    case SYNC_IN_TYPE:
        amp.inType = sync.inType;
        actionDispExpired();
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

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {

    }
}

static void actionRemapBtnLong(int16_t button)
{
    switch (button) {

    }
}

static void actionRemapEncoder(int16_t encCnt)
{
    if (amp.inType == IN_TUNER) {
        tunerStep(encCnt);
    } else {
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
}

void ampActionHandle(void)
{
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
            actionSetScreen(SCREEN_STANDBY, 0);
        }
        break;
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired();
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

static void prepareRadioView(RadioView *radio)
{
    radio->freq = tunerGet()->status.freq;
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

    RadioView radio;

    Spectrum *sp = spGet();
    SpMode spMode = sp->mode == SP_MODE_MIRROR ? SP_MODE_LEFT_MIRROR : SP_MODE_LEFT;

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
    case SCREEN_TUNER:
        prepareRadioView(&radio);
        canvasShowRadio(clear, &radio);
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
