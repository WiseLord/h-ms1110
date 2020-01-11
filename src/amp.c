#include "amp.h"

#include "gui/canvas.h"
#include "input.h"
#include "pins.h"
#include "rtc.h"
#include "settings.h"
#include "swtimers.h"

static Action action = {
    .type = ACTION_INIT,
    .screen = SCREEN_STANDBY,
};

static Screen screen = {
    .mode = SCREEN_STANDBY,
    .def = SCREEN_SPECTRUM,
};

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .inputStatus = 0x00,
};

static void actionSet(ActionType type, int16_t value)
{
    action.type = type;
    action.value = value;
}

static void actionSetScreen(ScreenMode screen, int16_t timeout)
{
    action.screen = screen;
    action.timeout = timeout;
}

static void actionDispExpired(ScreenMode scrMode)
{
    ScreenMode scrDef = screen.def;

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

static void actionResetSilenceTimer(void)
{
    int16_t silenceTimer = settingsGet(PARAM_SYSTEM_SIL_TIM);

    if (silenceTimer) {
        swTimSet(SW_TIM_SILENCE_TIMER, 1000 * 60 * silenceTimer + 999);
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
}

static void ampInit()
{
    ampPinMute(true);
    ampPinStby(true);

    amp.status = AMP_STATUS_STBY;
}

void ampExitStby(void)
{
    ampPinStby(false);     // Power on amplifier

    amp.status = AMP_STATUS_POWERED;
    swTimSet(SW_TIM_AMP_INIT, 600);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

//    screenSaveSettings();

//    audioSetMute(true);
//    audioSetPower(false);

//    inputDisable();

    ampInit();
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
//        pinsHwResetI2c();
//        i2cInit(I2C_AMP, 100000);

//        audioInit();
//        audioSetPower(true);
//        tunerInit();

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 500);
//        controlReportAll();
        break;
    case AMP_STATUS_HW_READY:
//        inputEnable();

        ampPinMute(false);

        amp.status = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 100);
        break;
    }
}


static void actionGetButtons(void)
{
    CmdBtn cmdBtn = getBtnCmd();

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
    case BTN_STBY:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case ENC_A:
        actionSet(ACTION_ENCODER, -1);
        break;
    case ENC_B:
        actionSet(ACTION_ENCODER, +1);
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(void)
{
    switch (action.value) {
    case BTN_STBY:
//        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case ENC_A:
        actionSet(ACTION_ENCODER, -1);
        break;
    case ENC_B:
        actionSet(ACTION_ENCODER, +1);
        break;
    default:
        break;
    }
}

static void actionRemapCommon(void)
{
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
}

void ampActionGet(void)
{
    actionSet(ACTION_NONE, 0);

    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }

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
}

void ampActionHandle(void)
{
    ScreenMode scrMode = screen.mode;

    action.timeout = 0;

    switch (action.type) {
    case ACTION_INIT:
        ampInit();
        swTimSet(SW_TIM_RTC_INIT, 500);
        break;
    case ACTION_INIT_HW:
        ampInitHw();
        actionResetSilenceTimer();
        break;
    case ACTION_INIT_RTC:
        rtcInit();
        break;
    case ACTION_STANDBY:
        if (action.value == FLAG_EXIT) {
            ampExitStby();
            actionSetScreen(SCREEN_TIME, 1000);
        } else {
            swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
            swTimSet(SW_TIM_SILENCE_TIMER, SW_TIM_OFF);
            swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

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

    screen.mode = action.screen;
    if (action.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, action.timeout);
    }
}

static bool screenCheckClear(void)
{
    bool clear = false;

    static ScreenMode scrPrev = SCREEN_END;

    if (screen.mode != scrPrev) {
        clear = true;
    }

    scrPrev = screen.mode;

    return clear;
}

void ampShowScreen()
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    switch (screen.mode) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear);
        break;
    case SCREEN_TIME:
        canvasShowTime(clear);
        break;
    case SCREEN_STANDBY:
        canvasShowStandby(clear);
        break;
    default:
        break;
    }

    glcdFbSync();
}
