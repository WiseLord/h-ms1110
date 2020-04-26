#include "amp.h"

#include <stddef.h>
#include <string.h>

#include "debug.h"
#include "gui/canvas.h"
#include "i2c.h"
#include "input.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "timers.h"
#include "tr/labels.h"
#include "tuner/tuner.h"
#include "utils.h"

static void actionGetMaster(void);
static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetTimers(void);

static void actionRemapBtnShort(void);
static void actionRemapBtnLong(void);
static void actionRemapCommon(void);
//static void actionRemapNavigate(void);
static void actionRemapEncoder(void);

static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

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

static AmpSync ampSync;
static AmpSync ampReport;

static SyncAction syncRxAction;

void ampSyncRxCb(int16_t bytes)
{
    (void)bytes;

    switch (ampSync.type) {
    case SYNC_ACTION:
        syncRxAction = ampSync.action;
        break;
    }
    ampSync.type = SYNC_NONE;
}

void ampSyncTxCb(int16_t bytes)
{
    if (ampReport.type != SYNC_NONE) {
        i2cBegin(I2C_SYNC, 0x28);
        for (size_t i = 0; i < sizeof(ampReport); i++) {
            i2cSend(I2C_SYNC, ampReport.data[i]);
        }
    } else {
        i2cBegin(I2C_SYNC, 0x28);
        for (size_t i = 0; i < sizeof (ampReport); i++) {
            i2cSend(I2C_SYNC, 0x00);
        }
    }
    memset(&ampReport, 0x00, sizeof(ampReport));
}

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
    ScreenType scrDef = amp.defScreen;

    rtcSetMode(RTC_NOEDIT);

    switch (scrMode) {
    case SCREEN_STANDBY:
        actionSetScreen(SCREEN_STANDBY, 1000);
        break;
    default:
        actionSetScreen(scrDef, 1000);
        break;
    }
}

static void ampPinMute(bool value)
{
    if (value) {
        SET(MUTE);
    } else {
        CLR(MUTE);
    }
}

static void ampPinStby(bool value)
{
    if (value) {
        CLR(STBY);
    } else {
        SET(STBY);
    }

    // Enable SWD interface in standby mode
    if (value) {
        LL_GPIO_AF_Remap_SWJ_NOJTAG();
    } else {
        LL_GPIO_AF_DisableRemap_SWJ();
    }
}

static void ampReadSettings(void)
{
    tunerReadSettings();
}

void ampExitStby(void)
{
    ampReadSettings();

    ampPinStby(false);      // Power on amplifier

    amp.status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 600);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
    swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SILENCE_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);

    settingsStore(PARAM_DISPLAY_DEF, amp.defScreen);

    ampPinMute(true);

    ampPinStby(true);

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
//        pinsHwResetI2c();
        i2cInit(I2C_AMP, 100000);

//        tunerInit();

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 500);
//        controlReportAll();
        break;
    case AMP_STATUS_HW_READY:
        ampPinMute(false);

        amp.status = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 100);
        break;
    }
}

static void actionGetMaster(void)
{
    if (syncRxAction.type != ACTION_NONE) {
        actionSet(syncRxAction.type, syncRxAction.value);
        syncRxAction.type = ACTION_NONE;
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

static void actionGetEncoder(void)
{
    int8_t encVal = inputGetEncoder();

    if (encVal) {
        actionSet(ACTION_ENCODER, encVal);
    }
}

static void actionGetTimers(void)
{
    if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    } else if (swTimGet(SW_TIM_AMP_INIT) == 0) {
        actionSet(ACTION_INIT_HW, 0);
    } else if (swTimGet(SW_TIM_STBY_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_SILENCE_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_RTC_INIT) == 0) {
        actionSet(ACTION_INIT_RTC, 0);
    }
}

static void actionRemapBtnShort(void)
{
    switch (action.value) {
    case BTN_MWFM:
//        action Set(ACTION_TUNER_BAND, action.value);
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(void)
{
    switch (action.value) {
    default:
        break;
    }
}

static void actionRemapEncoder(void)
{
    ScreenType scrMode = amp.screen;

    if (SCREEN_STANDBY == scrMode)
        return;

//    int16_t encCnt = action.value;

    switch (scrMode) {
    case SCREEN_TIME:
        if (rtcGetMode() == RTC_NOEDIT) {
        } else {
//            actionSet(ACTION_RTC_CHANGE, encCnt);
        }
        break;
//    case SCREEN_TEXTEDIT:
//        actionSet(ACTION_TEXTEDIT_CHANGE, encCnt);
//        break;
    default:
        break;
    }
}

static void actionRemapCommon(void)
{
    ScreenType scrMode = amp.screen;

    switch (action.type) {
    case ACTION_STANDBY:
        if (FLAG_SWITCH == action.value) {
            switch (amp.status) {
            case AMP_STATUS_STBY:
                action.value = FLAG_EXIT;
                break;
            case AMP_STATUS_ACTIVE:
                action.value = FLAG_ENTER;
                break;
            default:
                actionSet(ACTION_NONE, 0);
                break;
            }
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == scrMode &&
        (ACTION_STANDBY != action.type &&
         ACTION_INIT_RTC != action.type)) {
        actionSet(ACTION_NONE, 0);
    }
}

void ampInitMuteStby(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;

    GPIO_InitStruct.Pin = MUTE_Pin;
    LL_GPIO_Init(MUTE_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = STBY_Pin;
    LL_GPIO_Init(STBY_Port, &GPIO_InitStruct);

    ampPinMute(true);
    ampPinStby(true);
}

void ampInit(void)
{
    dbgInit();

    settingsInit();
    ampInitMuteStby();
    pinsInit();
    rtcInit();

    labelsInit();
    canvasInit();

    spInit();

    inputInit();
    rcInit();

    i2cInit(I2C_SYNC, 400000);
    i2cSetRxCb(I2C_SYNC, ampSyncRxCb);
    i2cSetTxCb(I2C_SYNC, ampSyncTxCb);
    i2cBegin(I2C_SYNC, 0x28);
    i2cSlaveTransmitReceive(I2C_SYNC, ampSync.data, sizeof(ampSync));

    ampReadSettings();

    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger
    swTimInit();

    swTimSet(SW_TIM_RTC_INIT, 500);

    amp.status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    while (1) {
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

void ampActionGet(void)
{
    actionSet(ACTION_NONE, 0);

    actionGetMaster();

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
    switch (action.type) {
    case ACTION_BTN_SHORT:
        actionRemapBtnShort();
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong();
        break;
    default:
        break;
    }

    actionRemapCommon();

    if (ACTION_ENCODER == action.type) {
        actionRemapEncoder();
    }
}

static void ampReportAction(ActionType type, int16_t value)
{
    ampReport.action.type = type;
    ampReport.action.value = value;

    ampReport.type = SYNC_ACTION;
}

void ampActionHandle(void)
{
    ScreenType scrMode = amp.screen;

    action.timeout = 0;

    switch (action.type) {
    case ACTION_INIT_HW:
        ampInitHw();
        break;
    case ACTION_INIT_RTC:
        rtcInit();
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

    case ACTION_OPEN_MENU:
        break;

    case ACTION_TUNER_BAND:
        // Do tuner band change
        ampReportAction(ACTION_TUNER_BAND, action.value);
        break;

    default:
        break;
    }

    screenSetMode(action.screen);

    if (action.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, action.timeout);
    }
}

void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    Spectrum *sp = spGet();
    Tune tune;

    switch (amp.screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, sp->mode, sp->peaks);
        break;
    case SCREEN_TIME:
        break;
    case SCREEN_INPUT:
        break;
    case SCREEN_STANDBY:
        break;
    case SCREEN_TUNE:
        tune.val = 20;
        canvasShowTune(clear, &tune);
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
