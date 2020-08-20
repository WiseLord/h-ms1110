#include "amp.h"

#include <stddef.h>
#include <string.h>

#include "audio/audio.h"
#include "debug.h"
#include "gui/canvas.h"
#include "i2c.h"
#include "input/analog.h"
#include "input.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "setup.h"
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
static void actionRemapPlayerBtnShort(int16_t button);
static void actionRemapPlayerBtnLong(int16_t button);

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

static bool rtcSyncRequired;

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
    .defScreen = SCREEN_SPECTRUM,
    .inputStatus = 0x00,
    .volume = 0,
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

static void rtcCb(void)
{
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

static void actionDispExpired(void)
{
    rtcSetMode(RTC_NOEDIT);

    switch (amp.screen) {
    case SCREEN_STANDBY:
    case SCREEN_SETUP:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(amp.defScreen, 0);
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
    syncMasterSendInType(AMP_TUNER_ADDR, amp.inType[input]);
    syncMasterSendInType(AMP_PLAYER_ADDR, amp.inType[input]);
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

    swTimSet(SW_TIM_AMP_INIT, 200);
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

void ampHandleStby(void)
{
    if (FLAG_SWITCH == action.value) {
        switch (amp.status) {
        case AMP_STATUS_STBY:
            action.value = FLAG_EXIT;
            break;
        case AMP_STATUS_ACTIVE:
            action.value = FLAG_ENTER;
            break;
        }
    }

    if (action.value == FLAG_EXIT) {
        ampExitStby();
        screenSet(SCREEN_TIME, 1000);
    } else {
        ampEnterStby();
        screenSet(SCREEN_STANDBY, 0);
    }
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000, 0x00);

        audioReset();

        audioSetPower(true);
        ampMute(true);

        amp.status = AMP_STATUS_HW_READY;
        swTimSet(SW_TIM_AMP_INIT, 300);

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
    swTimSet(SW_TIM_AMP_INIT, 300);
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

static void ampHandleSetup(void)
{
    Setup *setup = setupGet();
    if (SETUP_NULL == setup->active && SETUP_NULL == setup->child) {
        actionDispExpired();
    } else {
        screenSet(SCREEN_SETUP, 10000);
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

static bool isRemoteCmdRepeatable(RcCmd cmd)
{
    ScreenType scrMode = amp.screen;

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
                amp.screen = SCREEN_TUNE;
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
    if (swTimGet(SW_TIM_AMP_INIT) == 0) {
        actionSet(ACTION_INIT_HW, 0);
    } else if (swTimGet(SW_TIM_STBY_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_SILENCE_TIMER) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_RTC_INIT) == 0) {
        actionSet(ACTION_INIT_RTC, 0);
    } else if (swTimGet(SW_TIM_SOFT_VOLUME) == 0) {
        actionSet(ACTION_RESTORE_VOLUME, amp.volume);
    } else if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    }
}

static void spModeChange(int16_t value)
{
    Spectrum *sp = &amp.sp;

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

    amp.clearScreen = true;
    settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
    settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_STBY:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case BTN_IN_PREV:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_AUDIO_INPUT, -1);
        }
        break;
    case BTN_IN_NEXT:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, +1);
        } else {
            actionSet(ACTION_AUDIO_INPUT, +1);
        }
        break;

    case BTN_AUTO:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_BACK, 0);
        }
        break;
    case BTN_DEMO:
        if (SCREEN_SETUP == amp.screen) {
            Setup *setup = setupGet();
            if (setup->active <= SETUP_MAIN) { // has childs
                actionSet(ACTION_SETUP_SELECT, setup->child);
            }
        }
        break;
    case BTN_DISP_PREV:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_SP_MODE, -1);
        }
        break;
    case BTN_DISP_NEXT:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, +1);
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
            actionSet(ACTION_SETUP_SELECT, SETUP_MAIN);
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
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_SP_MODE, -1);
        }
        break;
    case BTN_RDS:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, +1);
        } else {
            actionSet(ACTION_SP_MODE, +1);
        }
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

static void actionRemapPlayerBtnShort(int16_t button)
{
    switch (button) {
    case BTN_OPEN:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_SP_MODE, -1);
        }
        break;
    case BTN_PLAYPAUSE:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, +1);
        } else {
            actionSet(ACTION_SP_MODE, +1);
        }
        break;
    case BTN_STOP:
        break;
    case BTN_REWIND:
        break;
    case BTN_REPEATE:
        break;
    case BTN_FORWARD:
        break;
    case BTN_AUDIO:
        break;
    case BTN_SUBTITLE:
        break;
    }
}

static void actionRemapPlayerBtnLong(int16_t button)
{
    switch (button) {
    case BTN_OPEN:
        break;
    case BTN_PLAYPAUSE:
        break;
    case BTN_STOP:
        break;
    case BTN_REWIND:
        break;
    case BTN_REPEATE:
        break;
    case BTN_FORWARD:
        break;
    case BTN_AUDIO:
        break;
    case BTN_SUBTITLE:
        break;
    }
}


static void actionRemapRemote(void)
{
    ScreenType scrMode = amp.screen;

    if (SCREEN_SETUP == scrMode) {
        Setup *setup = setupGet();
        if (setup->active == SETUP_REMOTE) {
            actionSet(ACTION_SETUP_CHANGE_CHILD, 0);
            return;
        }
    }

    if (SCREEN_STANDBY == scrMode && action.value != RC_CMD_STBY_SWITCH) {
        return;
    }

    switch (action.value) {
    case RC_CMD_STBY_SWITCH:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
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
    case SCREEN_SETUP:
        actionSet(ACTION_SETUP_CHANGE_CHILD, encCnt);
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
        amp.screen = SCREEN_TUNE;
        switch (scrMode) {
        case SCREEN_SPECTRUM:
            aProc->tune = AUDIO_TUNE_VOLUME;
            break;
        default:
            break;
        }
    }
}

static void actionRemapTunerEncoder(int16_t encCnt)
{
    (void)encCnt;
}

static void actionRemapCommon(void)
{
    switch (amp.screen) {
    case SCREEN_STANDBY:
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
        case ACTION_SETUP_SELECT:
        case ACTION_INIT_RTC:
            break;
        default:
            actionSet(ACTION_NONE, 0);
        }
        break;
    case SCREEN_SETUP:
        switch (action.type) {
        case ACTION_STANDBY:
        case ACTION_DISP_EXPIRED:
            actionSet(ACTION_DISP_EXPIRED, 0);
            break;
        case ACTION_SETUP_SELECT:
        case ACTION_SETUP_SWITCH_CHILD:
        case ACTION_SETUP_CHANGE_CHILD:
        case ACTION_SETUP_BACK:
            break;
        default:
            actionSet(ACTION_NONE, 0);
            break;
        }
        break;
    default:
        break;
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
    rtcInit();

    rtcSetCb(rtcCb);

    ampHandleSwd();

    labelsInit();
    canvasInit();

    inputInit();
    rcInit();

    i2cInit(I2C_SYNC, 400000, 0x00);

    ampReadSettings();

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
    uint8_t syncData[AMP_SYNC_DATASIZE];
    SyncType syncType;

    syncType = syncMasterReceive(AMP_TUNER_ADDR, syncData);
    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        return;
    }

    syncType = syncMasterReceive(AMP_PLAYER_ADDR, syncData);
    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        return;
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
    case ACTION_ENCODER:
        actionRemapEncoder(action.value);
        break;

    case ACTION_TUNER_BTN_SHORT:
        actionRemapTunerBtnShort(action.value);
        break;
    case ACTION_TUNER_BTN_LONG:
        actionRemapTunerBtnLong(action.value);
        break;
    case ACTION_TUNER_ENCODER:
        actionRemapTunerEncoder(action.value);
        break;

    case ACTION_PLAYER_BTN_SHORT:
        actionRemapPlayerBtnShort(action.value);
        break;
    case ACTION_PLAYER_BTN_LONG:
        actionRemapPlayerBtnLong(action.value);
        break;

    case ACTION_REMOTE:
        actionRemapRemote();
        break;

    default:
        break;
    }

    actionRemapCommon();
}

void ampActionHandle(void)
{
    ScreenType scrMode = amp.screen;

    AudioProc *aProc = audioGet();

    switch (action.type) {
    case ACTION_INIT_HW:
        ampInitHw();
        break;

    case ACTION_INIT_RTC:
        rtcInit();
        break;

    case ACTION_STANDBY:
        ampHandleStby();
        syncMasterSendAction(AMP_TUNER_ADDR, &action);
        syncMasterSendAction(AMP_PLAYER_ADDR, &action);
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
        actionDispExpired();
        break;

    case ACTION_SETUP_SELECT:
        setupSelect(action.value);
        ampHandleSetup();
        break;
    case ACTION_SETUP_CHANGE_CHILD:
        setupChangeChild(action.value);
        SetupType active = setupGet()->active;
        if (active == SETUP_TIME || active == SETUP_DATE) {
            uint32_t rtcRaw = rtcGetRaw();
            syncMasterSendTime(AMP_TUNER_ADDR, rtcRaw);
            syncMasterSendTime(AMP_PLAYER_ADDR, rtcRaw);
        }
        ampHandleSetup();
        break;
    case ACTION_SETUP_SWITCH_CHILD:
        setupSwitchChild(action.value);
        ampHandleSetup();
        break;
    case ACTION_SETUP_BACK:
        setupBack();
        ampHandleSetup();
        break;

    case ACTION_OPEN_MENU:
        if (scrMode == SCREEN_TUNE) {
            amp.clearScreen = true;
            actionNextAudioParam(aProc);
        } else {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        screenSet(SCREEN_TUNE, 5000);
        break;

    case ACTION_AUDIO_INPUT:
        ampSetInput(actionGetNextAudioInput((int8_t)action.value));
        break;
    case ACTION_AUDIO_PARAM_CHANGE:
        audioChangeTune(aProc->tune, (int8_t)action.value);
        if (aProc->tune == AUDIO_TUNE_VOLUME) {
            amp.volume = aProc->par.tune[AUDIO_TUNE_VOLUME].value;
        }
        if (aProc->par.mute) {
            ampMute(false);
        }
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;
    case ACTION_AUDIO_PARAM_SET:
        audioSetTune(aProc->tune, (int8_t)action.value);
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;


    case ACTION_SP_MODE:
        if (scrMode == SCREEN_SPECTRUM) {
            spModeChange(action.value);
        }
        screenSet(SCREEN_SPECTRUM, 3000);
        break;

    default:
        break;
    }

    if (scrMode != SCREEN_STANDBY && scrMode != SCREEN_SETUP) {
        // Reset silence timer on signal
//        if (spCheckSignal()) {
            actionResetSilenceTimer();
//        }
        // Reset silence timer on any user action
        if (action.type != ACTION_NONE &&
            action.type != ACTION_DISP_EXPIRED &&
            action.type != ACTION_STANDBY) {
            actionResetSilenceTimer();
        }
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
    static InputType _inType;

    AudioProc *aProc = audioGet();
    int8_t input = aProc->par.input;

    InputType inType = amp.inType[input];

    if (inType != _inType) {
        _inType = inType;
        amp.clearScreen = true;
    }

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

    TuneView tune;

    if (rtcSyncRequired) {
        uint32_t rtcRaw = rtcGetRaw();
        syncMasterSendTime(AMP_TUNER_ADDR, rtcRaw);
        syncMasterSendTime(AMP_PLAYER_ADDR, rtcRaw);
        rtcSyncRequired = false;
    }

    Spectrum *sp = &amp.sp;
    static Spectrum _sp;

    if ((sp->mode != _sp.mode) ||
        (sp->peaks != _sp.peaks)) {
        _sp.mode = sp->mode;
        _sp.peaks = sp->peaks;
        syncMasterSendSpectrum(AMP_TUNER_ADDR, sp);
    }

    Label label;

    switch (amp.screen) {
    case SCREEN_TIME:
        canvasShowTime(clear, true);
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
        prepareAudioInput(&label);
        canvasShowInput(clear, audioGet()->par.input, label);
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
}
