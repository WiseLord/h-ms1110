#include "amp.h"

#include <stddef.h>
#include <string.h>

#include "audio/audio.h"
#include "debug.h"
#include "gui/canvas.h"
#include "i2c.h"
#include "input/analog.h"
#include "input.h"
#include "pins.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "setup.h"
#include "spectrum.h"
#include "swtimers.h"
#include "timers.h"
#include "tr/labels.h"
#include "utils.h"

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetRemote(void);
#ifdef _INPUT_ANALOG
static void actionGetPots(void);
#endif
static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);

static void actionRemapTunerBtnShort(int16_t button);
static void actionRemapTunerBtnLong(int16_t button);

static void actionRemapRemote(void);
static void actionRemapCommon(void);
//static void actionRemapNavigate(void);

static void actionRemapEncoder(int16_t encCnt);
static void actionRemapTunerEncoder(int16_t encCnt);

static void ampActionSyncSlaves(void);
static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampHandleSwd(void);
static void ampScreenShow(void);

static uint32_t rtcSyncTime;
static bool rtcSyncRequired;

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


static void rtcCb(uint32_t time)
{
    rtcSyncTime = time;
    rtcSyncRequired = true;
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
    case SCREEN_SETUP:
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

static void ampMute(bool value)
{
    AudioProc *aProc = audioGet();
    AudioTuneItem *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];

    if (value) {
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
    } else {
        audioSetTune(AUDIO_TUNE_VOLUME, volItem->grid->min);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_ON);
    }

    ampPinMute(value);
    audioSetMute(value);
}

static void ampReadSettings(void)
{
    AudioProc *aProc = audioGet();
    AudioTuneItem *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];

    audioReadSettings();
    audioInitParam();

    amp.volume = volItem->value;
    volItem->value = volItem->grid->min;
}

void ampExitStby(void)
{
    ampReadSettings();

    ampPinStby(false);      // Power on amplifier

    inputSetPower(true);    // Power on input device

    amp.status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 600);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_ON);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
    swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SILENCE_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);

    settingsSet(PARAM_DISPLAY_DEF, amp.defScreen);
    settingsStore(PARAM_DISPLAY_DEF, amp.defScreen);

    ampMute(true);

    // Restore volume value before saving
    AudioProc *aProc = audioGet();
    AudioTuneItem *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];
    volItem->value = amp.volume;

    audioSetPower(false);

    inputDisable();

    ampPinStby(true);

    inputSetPower(false);   // Power off input device

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000);

        audioReset();

        audioSetPower(true);
        ampMute(true);

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 500);

        break;
    case AMP_STATUS_HW_READY:
        inputEnable();

        ampMute(false);

        amp.status = AMP_STATUS_ACTIVE;
        break;
    }
}

static void ampSetInput(int8_t value)
{
    ampMute(true);

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
        if (aProc->tune >= AUDIO_TUNE_END) {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
    } while (aProc->par.tune[aProc->tune].grid == NULL ||
             aProc->tune == AUDIO_TUNE_BASS ||
             aProc->tune == AUDIO_TUNE_TREBLE);
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

static bool isRemoteCmdRepeatable(RcCmd cmd)
{
    ScreenType scrMode = amp.screen;
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    switch (cmd) {
    case RC_CMD_VOL_UP:
    case RC_CMD_VOL_DOWN:
        return true;
    case RC_CMD_NAV_UP:
    case RC_CMD_NAV_DOWN:
        switch (scrMode) {
        case SCREEN_TUNE:
            return true;
        }
        break;
    case RC_CMD_NAV_LEFT:
    case RC_CMD_NAV_RIGHT:
        switch (scrMode) {
        case SCREEN_INPUT:
            if (inType == IN_TUNER) {
                return true;
            }
        }
        break;
    }

    return false;
}

static void actionGetRemote(void)
{
    RcData rcData = rcRead(true);
    static RcCmd cmdPrev = RC_CMD_END;

    if (rcData.ready) {
        swTimSet(SW_TIM_RC_NOACION, 200);

        RcCmd cmd = rcGetCmd(&rcData);
        int32_t repTime = swTimGet(SW_TIM_RC_REPEAT);

        if (cmd != cmdPrev) {
            actionSet(ACTION_REMOTE, (int16_t)cmd);
            swTimSet(SW_TIM_RC_REPEAT, 1000);
            cmdPrev = cmd;
        } else {
            if (isRemoteCmdRepeatable(cmd)) {
                if (repTime < 500) {
                    actionSet(ACTION_REMOTE, (int16_t)cmd);
                }
            } else {
                if (repTime == 0) {
                    actionSet(ACTION_REMOTE, (int16_t)cmd);
                    swTimSet(SW_TIM_RC_REPEAT, 1000);
                }
            }
        }
    } else {
        if (swTimGet(SW_TIM_RC_NOACION) == 0) {
            swTimSet(SW_TIM_RC_NOACION, SW_TIM_OFF);
            swTimSet(SW_TIM_RC_REPEAT, 0);
            cmdPrev = RC_CMD_END;
        }
    }
}

#ifdef _INPUT_ANALOG

static void actionGetPots(void)
{
    static int8_t potPrev[AIN_POT_END];

    AudioProc *aProc = audioGet();
    const AudioGrid *grid = aProc->par.tune[AUDIO_TUNE_BASS].grid;
    uint16_t zoneCnt = grid->max - grid->min + 1;

    for (AinChannel ain = AIN_POT_A; ain < AIN_POT_END; ain++) {
        int8_t pot = inputAnalogGetPotZone(ain, zoneCnt);
        if (pot != potPrev[ain]) {
            if (amp.status == AMP_STATUS_ACTIVE) {
                screenSetMode(SCREEN_TUNE);
                switch (ain) {
                case  AIN_POT_A:
                    if (aProc->tune != AUDIO_TUNE_BASS) {
                        aProc->tune = AUDIO_TUNE_BASS;
                        amp.clearScreen = true;
                    }
                    actionSet(ACTION_AUDIO_PARAM_SET, pot + grid->min);
                    break;
                case AIN_POT_B:
                    if (aProc->tune != AUDIO_TUNE_TREBLE) {
                        aProc->tune = AUDIO_TUNE_TREBLE;
                        amp.clearScreen = true;
                    }
                    actionSet(ACTION_AUDIO_PARAM_SET, pot + grid->min);
                    break;
                default:
                    break;
                }
            }
            potPrev[ain] = pot;
        }
    }
}

#endif // _INPUT_ANALOG

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
    } else if (swTimGet(SW_TIM_SOFT_VOLUME) == 0) {
        actionSet(ACTION_RESTORE_VOLUME, amp.volume);
    }
}

static void spModeChange(int16_t value)
{
    Spectrum *sp = spGet();

    if (value > 1) {

    } else if (value < 1) {

    }

    if (++sp->mode >= (sp->peaks ? SP_MODE_END : SP_MODE_WATERFALL)) {
        sp->mode = SP_MODE_STEREO;
        sp->peaks = !sp->peaks;
        settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);
    }
    amp.clearScreen = true;
    settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_STBY:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case BTN_IN_PREV:
        actionSet(ACTION_AUDIO_INPUT, -1);
        break;
    case BTN_IN_NEXT:
        actionSet(ACTION_AUDIO_INPUT, +1);
        break;

    case BTN_AUTO:
        break;
    case BTN_DEMO:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP, setupGet()->active);
        }
        break;
    case BTN_DISP_PREV:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_MOVE, -1);
        } else {
            actionSet(ACTION_SP_MODE, -1);
        }
        break;
    case BTN_DISP_NEXT:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_MOVE, +1);
        } else {
            actionSet(ACTION_SP_MODE, +1);
        }
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(int16_t button)
{
    switch (button) {
    case BTN_STBY:
        if (SCREEN_STANDBY == amp.screen) {
            actionSet(ACTION_SETUP, SETUP_MAIN);
        }
        break;
    case BTN_IN_PREV:
        break;
    case BTN_IN_NEXT:
        break;

    case BTN_AUTO:
        break;
    case BTN_DEMO:
        break;
    case BTN_DISP_PREV:
        break;
    case BTN_DISP_NEXT:
        break;
    default:
        break;
    }
}

static void actionRemapTunerBtnShort(int16_t button)
{
    switch (button) {
    case BTN_MWFM:
        break;
    case BTN_RDS:
        break;
    case BTN_ENC:
        break;
    case BTN_1:
    case BTN_2:
    case BTN_3:
    case BTN_4:
    case BTN_5:
    case BTN_6:
    case BTN_7:
    case BTN_8:
    case BTN_9:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    default:
        break;
    }
}

static void actionRemapTunerBtnLong(int16_t button)
{
    switch (button) {
    case BTN_MWFM:
        break;
    case BTN_RDS:
        break;
    case BTN_ENC:
        action.type = ACTION_OPEN_MENU;
        break;
    case BTN_1:
    case BTN_2:
    case BTN_3:
    case BTN_4:
    case BTN_5:
    case BTN_6:
    case BTN_7:
    case BTN_8:
    case BTN_9:
        break;
    default:
        break;
    }
}


static void actionRemapRemote(void)
{
    ScreenType scrMode = amp.screen;

    AudioProc *aProc = audioGet();

    if (SCREEN_STANDBY == scrMode &&
        action.value != RC_CMD_STBY_SWITCH)
        return;

    switch (action.value) {
    case RC_CMD_STBY_SWITCH:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;

    case RC_CMD_MUTE:
        actionSet(ACTION_AUDIO_MUTE, FLAG_SWITCH);
        break;
    case RC_CMD_VOL_UP:
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_VOL_DOWN:
        actionSet(ACTION_ENCODER, -1);
        break;

    case RC_CMD_MENU:
        action.type = ACTION_OPEN_MENU;
        break;

//    case RC_CMD_CHAN_NEXT:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_NEXT_TRACK);
//        break;
//    case RC_CMD_CHAN_PREV:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_PREV_TRACK);
//        break;

//    case RC_CMD_DIG_0:
//    case RC_CMD_DIG_1:
//    case RC_CMD_DIG_2:
//    case RC_CMD_DIG_3:
//    case RC_CMD_DIG_4:
//    case RC_CMD_DIG_5:
//    case RC_CMD_DIG_6:
//    case RC_CMD_DIG_7:
//    case RC_CMD_DIG_8:
//    case RC_CMD_DIG_9:
//        actionSet(ACTION_DIGIT, action.value - RC_CMD_DIG_0);
//        break;

    case RC_CMD_IN_NEXT:
        actionSet(ACTION_AUDIO_INPUT, +1);
        break;

//    case RC_CMD_NAV_OK:
//    case RC_CMD_NAV_BACK:
//    case RC_CMD_NAV_RIGHT:
//    case RC_CMD_NAV_UP:
//    case RC_CMD_NAV_LEFT:
//    case RC_CMD_NAV_DOWN:
//        actionSet(ACTION_NAVIGATE, action.value);
//        break;

    case RC_CMD_BASS_UP:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_BASS) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_BASS_DOWN:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_BASS) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_BASS;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_MIDDLE_UP:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_MIDDLE) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_MIDDLE_DOWN:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_MIDDLE) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_MIDDLE;
        actionSet(ACTION_ENCODER, -1);
        break;
    case RC_CMD_TREBLE_UP:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_TREBLE) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_TREBLE;
        actionSet(ACTION_ENCODER, +1);
        break;
    case RC_CMD_TREBLE_DOWN:
        screenSetMode(SCREEN_TUNE);
        if (aProc->tune != AUDIO_TUNE_TREBLE) {
            amp.clearScreen = true;
        }
        aProc->tune = AUDIO_TUNE_TREBLE;
        actionSet(ACTION_ENCODER, -1);
        break;

    case RC_CMD_LOUDNESS:
        actionSet(ACTION_AUDIO_LOUDNESS, FLAG_SWITCH);
        break;
//    case RC_CMD_SURROUND:
//        actionSet(ACTION_AUDIO_SURROUND, FLAG_SWITCH);
//        break;
//    case RC_CMD_EFFECT_3D:
//        actionSet(ACTION_AUDIO_EFFECT3D, FLAG_SWITCH);
//        break;
    case RC_CMD_TONE_BYPASS:
        actionSet(ACTION_AUDIO_BYPASS, FLAG_SWITCH);
        break;

//    case RC_CMD_TIME:
//        actionSet(ACTION_RTC_MODE, 0);
//        break;

//    case RC_CMD_STOP:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_STOP);
//        break;
//    case RC_CMD_PLAY_PAUSE:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_PLAY);
//        break;
//    case RC_CMD_REW:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_REWIND);
//        break;
//    case RC_CMD_FWD:
//        actionSet(ACTION_MEDIA, HIDMEDIAKEY_FFWD);
//        break;
//    case RC_CMD_TIMER:
//        actionSet(ACTION_TIMER, 0);
//        break;
//    case RC_CMD_SP_MODE:
//        actionSet(ACTION_SP_MODE, 0);
//        break;
//    case RC_CMD_SCR_DEF:
//        actionSet(ACTION_SCR_DEF, 0);
//        break;
    default:
        break;
    }
}

static void actionRemapEncoder(int16_t encCnt)
{
    ScreenType scrMode = amp.screen;
    AudioProc *aProc = audioGet();

    if (SCREEN_STANDBY == scrMode) {
        return;
    }

    switch (scrMode) {
    case SCREEN_TIME:
        if (rtcGetMode() == RTC_NOEDIT) {
            actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        } else {
//            actionSet(ACTION_RTC_CHANGE, encCnt);
        }
        break;
    default:
        if (aProc->tune == AUDIO_TUNE_BASS || aProc->tune == AUDIO_TUNE_TREBLE) {
            amp.clearScreen = true;
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        actionSet(ACTION_AUDIO_PARAM_CHANGE, encCnt);
        break;
    }

    if (ACTION_AUDIO_PARAM_CHANGE == action.type) {
        screenSetMode(SCREEN_TUNE);
        switch (scrMode) {
        case SCREEN_SPECTRUM:
//        case SCREEN_AUDIO_FLAG:
        case SCREEN_INPUT:
            aProc->tune = AUDIO_TUNE_VOLUME;
            break;
        default:
            break;
        }
    }
}

static void actionRemapTunerEncoder(int16_t encCnt)
{
    actionRemapEncoder(encCnt);
}

static void actionRemapCommon(void)
{
    AudioProc *aProc = audioGet();

    switch (action.type) {
    case ACTION_STANDBY:
        if (SCREEN_SETUP == amp.screen) {
            action.value = FLAG_ENTER;
        } else if (FLAG_SWITCH == action.value) {
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

    if (SCREEN_STANDBY == amp.screen &&
        (ACTION_STANDBY != action.type &&
         ACTION_SETUP != action.type &&
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

    rtcSetCb(rtcCb);

    labelsInit();
    canvasInit();

    spInit();

    inputInit();
    rcInit();

    i2cInit(I2C_SYNC, 400000);

    ampReadSettings();

    timerInit(TIM_SPECTRUM, 99, 35); // 20kHz timer: ADC conversion trigger
    swTimInit();

    inputSetPower(false);    // Power off input device

    swTimSet(SW_TIM_RTC_INIT, 500);

    amp.status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    while (1) {
        ampHandleSwd();
        ampActionSyncSlaves();

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

static void ampActionSyncSlaves(void)
{
    AmpSync sync;

    sync.type = SYNC_NONE;

    syncMasterReceive(AMP_TUNER_ADDR, &sync);

    switch (sync.type) {
    case SYNC_ACTION:
        action = sync.action;
        break;
    }
}

static void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        actionGetButtons();
    }

    if (ACTION_NONE == action.type) {
        actionGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetRemote();
    }

#ifdef _INPUT_ANALOG
    if (ACTION_NONE == action.type) {
        actionGetPots();
    }
#endif // _INPUT_ANALOG

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
        actionRemapBtnShort(action.value);
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong(action.value);
        break;
    case ACTION_REMOTE:
        actionRemapRemote();
        break;

    case ACTION_TUNER_BTN_SHORT:
        actionRemapTunerBtnShort(action.value);
        break;
    case ACTION_TUNER_BTN_LONG:
        actionRemapTunerBtnLong(action.value);
        break;
    default:
        break;
    }

    actionRemapCommon();

    if (ACTION_ENCODER == action.type) {
        actionRemapEncoder(action.value);
    }
    if (ACTION_TUNER_ENCODER == action.type) {
        actionRemapTunerEncoder(action.value);
    }
}

void ampActionHandle(void)
{
    ScreenType scrMode = amp.screen;

    AudioProc *aProc = audioGet();

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
            ampEnterStby();
            actionDispExpired(SCREEN_STANDBY);
        }
        syncMasterSendAction(AMP_TUNER_ADDR, &action);
        break;

    case ACTION_RESTORE_VOLUME:
        if (aProc->par.tune[AUDIO_TUNE_VOLUME].value < action.value) {
            audioChangeTune(AUDIO_TUNE_VOLUME, +1);
            swTimSet(SW_TIM_SOFT_VOLUME, 25);
        } else {
            swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        }
        break;
    case ACTION_DISP_EXPIRED:
        actionDispExpired(scrMode);
        break;

    case ACTION_SETUP:
        setupSelect(action.value);
        actionSetScreen(SCREEN_SETUP, 10000);
        break;
    case ACTION_SETUP_MOVE:
        setupMove(action.value);
        actionSetScreen(SCREEN_SETUP, 10000);
        break;

    case ACTION_OPEN_MENU:
        if (scrMode == SCREEN_TUNE) {
            amp.clearScreen = true;
            actionNextAudioParam(aProc);
        } else {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        actionSetScreen(SCREEN_TUNE, 5000);
        break;

    case ACTION_AUDIO_INPUT:
        if (scrMode == SCREEN_INPUT) {
            ampSetInput(actionGetNextAudioInput((int8_t)action.value));
//            controlReportAudioInput();
//            controlReportAudioTune(AUDIO_TUNE_GAIN);
        }
        amp.clearScreen = true;
        actionSetScreen(SCREEN_INPUT, 5000);
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        audioChangeTune(aProc->tune, (int8_t)action.value);
        if (aProc->tune == AUDIO_TUNE_VOLUME) {
            amp.volume = aProc->par.tune[AUDIO_TUNE_VOLUME].value;
        }
        if (aProc->par.mute) {
            ampMute(false);
        }
        actionSetScreen(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;
    case ACTION_AUDIO_PARAM_SET:
        audioSetTune(aProc->tune, (int8_t)action.value);
        actionSetScreen(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;


    case ACTION_SP_MODE:
        if (scrMode == SCREEN_SPECTRUM) {
            spModeChange(action.value);
        }
        actionSetScreen(SCREEN_SPECTRUM, 3000);
        break;

    default:
        break;
    }

    // Reset silence timer on any user action
    if (action.type != ACTION_NONE && action.type != ACTION_DISP_EXPIRED) {
        actionResetSilenceTimer();
    }

    // Reset silence timer on signal
    if (scrMode != SCREEN_STANDBY) {
        if (spCheckSignal()) {
            actionResetSilenceTimer();
        }
    }

    if (action.type != ACTION_NONE) {
        screenSetMode(action.screen);
    }

    if (action.timeout > 0) {
        swTimSet(SW_TIM_DISPLAY, action.timeout);
    }

    actionSet(ACTION_NONE, 0);
}

static void prepareAudioTune(TuneView *tune)
{
    AudioProc *aProc = audioGet();
    AudioTuneItem *aItem = &aProc->par.tune[aProc->tune];

    tune->value = aItem->value;
    tune->min = aItem->grid->min;
    tune->max = aItem->grid->max;

    tune->label = LABEL_VOLUME + (aProc->tune - AUDIO_TUNE_VOLUME);
}

static void prepareAudioInput (Label *label)
{
    AudioProc *aProc = audioGet();

    InputType inType = aProc->par.inType[aProc->par.input];

    *label = LABEL_IN_TUNER + (inType - IN_TUNER);
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

static void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    Spectrum *sp = spGet();
    TuneView tune;

    Label label;

    if (rtcSyncRequired) {
        syncMasterSendTime(AMP_TUNER_ADDR, rtcSyncTime);
        rtcSyncRequired = false;
    }

    switch (amp.screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, sp->mode, sp->peaks);
        break;
    case SCREEN_TIME:
        canvasShowTime(clear, true);
        break;
    case SCREEN_INPUT:
        prepareAudioInput(&label);
        canvasShowInput(clear, label);
        break;
    case SCREEN_STANDBY:
        canvasShowTime(clear, false);
        break;
    case SCREEN_TUNE:
        prepareAudioTune(&tune);
        canvasShowTune(clear, &tune);
        break;
    case SCREEN_SETUP:
        canvasShowSetup(clear);
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
