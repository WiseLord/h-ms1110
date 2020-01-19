#include "amp.h"

#include <stddef.h>

#include "audio/audio.h"
#include "i2c.h"
#include "input.h"
#include "pins.h"
#include "rtc.h"
#include "settings.h"
#include "swtimers.h"
#include "timers.h"

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetRemote(void);
static void actionGetTimers(void);
static void actionRemapBtnShort(void);
static void actionRemapBtnLong(void);
static void actionRemapRemote(void);
static void actionRemapCommon(void);
static void actionRemapEncoder(void);

static Action action = {
    .type = ACTION_STANDBY,
    .screen = SCREEN_STANDBY,
    .value = FLAG_ENTER,
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

static void actionSetScreen(ScrMode screen, int16_t timeout)
{
    action.screen = screen;
    action.timeout = timeout;
}

static void actionDispExpired(ScrMode scrMode)
{
    Screen *screen = screenGet();

    ScrMode scrDef = screen->def;

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

static void inputDisable(void)
{
    // TODO: Power off current input device
}

static void inputEnable(void)
{
    // TODO: Power on current input device
}

static void inputSetPower(bool value)
{
    AudioProc *aProc = audioGet();
    int8_t input = aProc->par.input;

    if (value) {
        amp.inputStatus = (uint8_t)(1 << input);
    } else {
        amp.inputStatus = 0x00;
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

void ampExitStby(void)
{
    audioReadSettings();
//    tunerReadSettings();

    ampPinStby(false);      // Power on amplifier

    inputSetPower(true);    // Power on input device

    amp.status = AMP_STATUS_POWERED;
    swTimSet(SW_TIM_AMP_INIT, 600);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    screenSaveSettings();

    audioSetMute(true);
    ampPinMute(true);
    audioSetPower(false);

    inputDisable();

    ampPinStby(true);

    inputSetPower(false);   // Power off input device

    amp.status = AMP_STATUS_STBY;
//    controlReportAmpStatus();
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
//        pinsHwResetI2c();
        i2cInit(I2C_AMP, 100000);

        audioInit();
        audioSetPower(true);
//        tunerInit();

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 500);
//        controlReportAll();
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();

        ampPinMute(false);
        audioSetMute(false);

        amp.status = AMP_STATUS_ACTIVE;

        swTimSet(SW_TIM_INPUT_POLL, 100);
        break;
    }
}

static void ampSetInput(int8_t value)
{
    swTimSet(SW_TIM_INPUT_POLL, SW_TIM_OFF);

    audioSetMute(true);
    ampPinMute(true);

    inputDisable();
    inputSetPower(false);
    audioSetInput(value);
    inputSetPower(true);

    amp.status = AMP_STATUS_HW_READY;
    swTimSet(SW_TIM_AMP_INIT, 400);
}

static void actionNextAudioParam(AudioProc *aProc)
{
    do {
        aProc->tune++;
        if (aProc->tune >= AUDIO_TUNE_END)
            aProc->tune = AUDIO_TUNE_VOLUME;
    } while (aProc->par.tune[aProc->tune].grid == NULL && aProc->tune != AUDIO_TUNE_VOLUME);
}

static int8_t actionGetNextAudioInput(int8_t diff)
{
    AudioProc *aProc = audioGet();

    int8_t ret = aProc->par.input + diff;
    int8_t inCnt = aProc->par.inCnt;

    if (ret < 0) {
        ret = inCnt - 1;
    } else if (ret >= inCnt) {
        ret = 0;
    }

    return ret;
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

static void actionGetEncoder(void)
{
    int8_t encVal = getEncoder();

    if (encVal) {
        actionSet(ACTION_ENCODER, encVal);
    }
}

static void actionGetRemote(void)
{

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
        actionSet(ACTION_OPEN_MENU, 0);
        break;
    case BTN_IN_PREV:
        actionSet(ACTION_AUDIO_INPUT, -1);
        break;
    case BTN_IN_NEXT:
        actionSet(ACTION_AUDIO_INPUT, +1);
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

static void actionRemapRemote(void)
{

}

static void actionRemapEncoder(void)
{
    ScrMode scrMode = screenGet()->mode;
    AudioProc *aProc = audioGet();

    if (SCREEN_STANDBY == scrMode)
        return;

    int16_t encCnt = action.value;

    switch (scrMode) {
    case SCREEN_TIME:
        if (rtcGetMode() == RTC_NOEDIT) {
            actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        } else {
//            actionSet(ACTION_RTC_CHANGE, encCnt);
        }
        break;
//    case SCREEN_MENU:
//        actionSet(ACTION_MENU_CHANGE, encCnt);
//        break;
//    case SCREEN_TEXTEDIT:
//        actionSet(ACTION_TEXTEDIT_CHANGE, encCnt);
//        break;
    default:
        actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        break;
    }

    if (ACTION_AUDIO_PARAM_CHANGE == action.type) {
        screenSetMode(SCREEN_AUDIO_PARAM);
        switch (scrMode) {
        case SCREEN_SPECTRUM:
//        case SCREEN_AUDIO_FLAG:
//        case SCREEN_AUDIO_INPUT:
            aProc->tune = AUDIO_TUNE_VOLUME;
            break;
        default:
            break;
        }
    }
}

static void actionRemapCommon(void)
{
    ScrMode scrMode = screenGet()->mode;
    AudioProc *aProc = audioGet();

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
    case ACTION_AUDIO_MUTE:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.mute;
        }
        break;
    case ACTION_AUDIO_LOUDNESS:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.loudness;
        }
        break;
    case ACTION_AUDIO_BYPASS:
        if (FLAG_SWITCH == action.value) {
            action.value = !aProc->par.bypass;
        }
        break;
    default:
        break;
    }

    if (SCREEN_STANDBY == scrMode &&
        (ACTION_STANDBY != action.type &&
         ACTION_REMOTE != action.type &&
         ACTION_INIT_RTC != action.type)) {
        actionSet(ACTION_NONE, 0);
    }
}

void ampInitMuteStby(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = MUTE_Pin;
    LL_GPIO_Init(MUTE_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = STBY_Pin;
    LL_GPIO_Init(STBY_Port, &GPIO_InitStruct);

    ampPinMute(true);
    ampPinStby(true);
}

void ampInit(void)
{
    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger
    swTimInit();

    ampPinMute(true);
    ampPinStby(true);

    inputSetPower(false);    // Power off input device

    swTimSet(SW_TIM_RTC_INIT, 500);

    amp.status = AMP_STATUS_STBY;
}

void ampActionGet(void)
{
    actionSet(ACTION_NONE, 0);

    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetRemote();
    }

    if (ACTION_NONE == action.type) {
        ScrMode scrMode = screenGet()->mode;

        if (scrMode == SCREEN_STANDBY && rtcCheckAlarm()) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
        }
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
    case ACTION_REMOTE:
        actionRemapRemote();
        break;
    default:
        break;
    }

    actionRemapCommon();

    if (ACTION_ENCODER == action.type) {
        actionRemapEncoder();
    }
}

void ampActionHandle(void)
{
    Screen *screen = screenGet();
    ScrMode scrMode = screen->mode;

    AudioProc *aProc = audioGet();
//    InputType inType = aProc->par.inType[aProc->par.input];

    action.timeout = 0;

    switch (action.type) {
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

    case ACTION_OPEN_MENU:
        if (scrMode == SCREEN_AUDIO_PARAM) {
            screenToClear();
            actionNextAudioParam(aProc);
        } else {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        actionSetScreen(SCREEN_AUDIO_PARAM, 5000);
        break;

    case ACTION_AUDIO_INPUT:
        if (scrMode == SCREEN_AUDIO_INPUT) {
            ampSetInput(actionGetNextAudioInput((int8_t)action.value));
//            controlReportAudioInput();
//            controlReportAudioTune(AUDIO_TUNE_GAIN);
        }
        screenToClear();
        actionSetScreen(SCREEN_AUDIO_INPUT, 5000);
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        audioChangeTune(aProc->tune, (int8_t)(action.value));
        actionSetScreen(SCREEN_AUDIO_PARAM, 3000);
//        controlReportAudioTune(aProc->tune);
        break;

    default:
        break;
    }

    screenSetMode(action.screen);
    if (action.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, action.timeout);
    }
}

void TIM_SPECTRUM_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_SPECTRUM)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_SPECTRUM);

        // Callbacks
//        spConvertADC();
    }
}
