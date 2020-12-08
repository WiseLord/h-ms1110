#include "amp.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "audio/audio.h"
#include "debug.h"
#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input/analog.h"
#include "input.h"
#include "mpc.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "setup.h"
#include "swtimers.h"
#include "timers.h"
#include "tr/labels.h"
#include "tunersync.h"
#include "utils.h"

typedef uint8_t SyncFlags;
enum {
    SYNC_FLAG_RTC       = 0x01,
    SYNC_FLAG_SPECTRUM  = 0x02,
};

typedef struct {
    SyncFlags syncFlags;

    bool clearScreen;

    ScreenType prevScreen;

    uint8_t inputStatus;
    int8_t volume;
    uint8_t silenceTimer;

    Spectrum sp;
    InputType inType;
    Action syncAction;
} AmpPriv;

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetRemote(void);
static void actionGetPots(void);
static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);
static void actionRemapRemote(void);
static void actionRemapEncoder(int16_t encCnt);

static void actionRemapCommon(void);

static void ampGetFromSlaves(void);
static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampSendToSlaves(void);
static void ampScreenShow(void);

static AmpPriv ampPriv;

static const InputType inTypes[MAX_INPUTS] = {
    IN_TUNER,
    IN_MPD,
    IN_NULL,
    IN_NULL,
    IN_AUX1,
    IN_AUX2,
    IN_NULL,
    IN_NULL,
};

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

static void rtcCb(void)
{
    ampPriv.syncFlags |= SYNC_FLAG_RTC;
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
    case SCREEN_SETUP:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(SCREEN_INPUT, 0);
        break;
    }
}

static void actionResetSilenceTimer(void)
{
    if (ampPriv.silenceTimer) {
        swTimSet(SW_TIM_SILENCE_TIMER, 1000 * 60 * ampPriv.silenceTimer + 999);
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
        ampPriv.inputStatus = (uint8_t)(1 << input);
    } else {
        ampPriv.inputStatus = 0x00;
    }

    amp.inType = inTypes[input];
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

    audioReadSettings(AUDIO_IC_TDA7719);

    ampPriv.silenceTimer = settingsRead(PARAM_SYSTEM_SIL_TIM, 0);

    ampPriv.volume = volItem->value;
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
    swTimSet(SW_TIM_STBY_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SILENCE_TIMER, SW_TIM_OFF);
    swTimSet(SW_TIM_SP_CONVERT, SW_TIM_OFF);

    ampMute(true);

    // Restore volume value before saving
    AudioProc *aProc = audioGet();
    AudioTuneItem *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];
    volItem->value = ampPriv.volume;

    audioSetPower(false);

    inputDisable();

    inputSetPower(false);   // Power off input device

    amp.status = AMP_STATUS_STBY;
    swTimSet(SW_TIM_AMP_INIT, 1000);
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

    ampPriv.syncAction = action;

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
    case AMP_STATUS_STBY:
        swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
        ampPinStby(true);
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
    case RC_CMD_NAV_LEFT:
    case RC_CMD_NAV_RIGHT:
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
                        ampPriv.clearScreen = true;
                    }
                    actionSet(ACTION_AUDIO_SET_PARAM, pot + grid->min);
                    break;
                case AIN_POT_B:
                    if (aProc->tune != AUDIO_TUNE_TREBLE) {
                        aProc->tune = AUDIO_TUNE_TREBLE;
                        ampPriv.clearScreen = true;
                    }
                    actionSet(ACTION_AUDIO_SET_PARAM, pot + grid->min);
                    break;
                default:
                    break;
                }
            }
            potPrev[ain] = pot;
        }
    }
}

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
        actionSet(ACTION_RESTORE_VOLUME, ampPriv.volume);
    } else if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    }
}

static void actionRemapBtnShort(int16_t button)
{
    switch (button) {
    case BTN_AMP_STBY:
        actionSet(ACTION_STANDBY, FLAG_SWITCH);
        break;
    case BTN_AMP_IN_PREV:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_AUDIO_SELECT_INPUT, -1);
        }
        break;
    case BTN_AMP_IN_NEXT:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, +1);
        } else {
            actionSet(ACTION_AUDIO_SELECT_INPUT, +1);
        }
        break;

    case BTN_PLAYER_OPEN:
        actionSet(ACTION_MEDIA, MEDIAKEY_EJECT);
        break;
    case BTN_PLAYER_PLAYPAUSE:
        actionSet(ACTION_MEDIA, MEDIAKEY_PAUSE);
        break;
    case BTN_PLAYER_STOP:
        actionSet(ACTION_MEDIA, MEDIAKEY_STOP);
        break;
    case BTN_PLAYER_REWIND:
        actionSet(ACTION_MEDIA, MEDIAKEY_PREV);
        break;
    case BTN_PLAYER_REPEATE:
        actionSet(ACTION_MEDIA, MEDIAKEY_REPEAT);
        break;
    case BTN_PLAYER_FORWARD:
        actionSet(ACTION_MEDIA, MEDIAKEY_NEXT);
        break;
    case BTN_PLAYER_AUDIO:
        if (SCREEN_SETUP == amp.screen) {
            actionSet(ACTION_SETUP_BACK, 0);
        } else {
            action.type = ACTION_AUDIO_MENU;
        }
        break;
    case BTN_PLAYER_SUBTITLE:
        if (SCREEN_SETUP == amp.screen) {
            Setup *setup = setupGet();
            if (setup->active <= SETUP_MAIN) { // has childs
                actionSet(ACTION_SETUP_SELECT, setup->child);
            }
        }
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(int16_t button)
{
    switch (button) {
    case BTN_AMP_STBY:
        if (SCREEN_STANDBY == amp.screen) {
            actionSet(ACTION_SETUP_SELECT, SETUP_MAIN);
        }
        break;
    case BTN_AMP_IN_PREV:
        break;
    case BTN_AMP_IN_NEXT:
        break;

    case BTN_PLAYER_OPEN:
        actionSet(ACTION_MEDIA, MEDIAKEY_INJECT);
        break;
    case BTN_PLAYER_PLAYPAUSE:
        actionSet(ACTION_MEDIA, MEDIAKEY_SINGLE);
        break;
    case BTN_PLAYER_STOP:
        actionSet(ACTION_MEDIA, MEDIAKEY_CONSUME);
        break;
    case BTN_PLAYER_REWIND:
        actionSet(ACTION_MEDIA, MEDIAKEY_REWIND);
        break;
    case BTN_PLAYER_REPEATE:
        actionSet(ACTION_MEDIA, MEDIAKEY_RANDOM);
        break;
    case BTN_PLAYER_FORWARD:
        actionSet(ACTION_MEDIA, MEDIAKEY_FFWD);
        break;
    case BTN_PLAYER_AUDIO:
        break;
    case BTN_PLAYER_SUBTITLE:
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
    case RC_CMD_VOL_UP:
        amp.screen = SCREEN_TUNE;
        audioGet()->tune = AUDIO_TUNE_VOLUME;
        actionSet(ACTION_AUDIO_SELECT_PARAM, +1);
        break;
    case RC_CMD_VOL_DOWN:
        amp.screen = SCREEN_TUNE;
        audioGet()->tune = AUDIO_TUNE_VOLUME;
        actionSet(ACTION_AUDIO_SELECT_PARAM, -1);
        break;
    case RC_CMD_MUTE:
        actionSet(ACTION_AUDIO_MUTE, FLAG_SWITCH);
        break;


    case RC_CMD_IN_PREV:
        actionSet(ACTION_AUDIO_SELECT_INPUT, -1);
        break;
    case RC_CMD_IN_NEXT:
        actionSet(ACTION_AUDIO_SELECT_INPUT, +1);
        break;

    case RC_CMD_PLAY:
        actionSet(ACTION_MEDIA, MEDIAKEY_PAUSE);
        break;
    case RC_CMD_PAUSE:
        actionSet(ACTION_MEDIA, MEDIAKEY_PAUSE);
        break;
    case RC_CMD_STOP:
        actionSet(ACTION_MEDIA, MEDIAKEY_STOP);
        break;
    case RC_CMD_PREV:
        actionSet(ACTION_MEDIA, MEDIAKEY_PREV);
        break;
    case RC_CMD_NEXT:
        actionSet(ACTION_MEDIA, MEDIAKEY_NEXT);
        break;
    case RC_CMD_REW:
        actionSet(ACTION_MEDIA, MEDIAKEY_REWIND);
        break;
    case RC_CMD_FWD:
        actionSet(ACTION_MEDIA, MEDIAKEY_FFWD);
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
            ampPriv.clearScreen = true;
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        actionSet(ACTION_AUDIO_SELECT_PARAM, encCnt);
        break;
    }

    if (ACTION_AUDIO_SELECT_PARAM == action.type) {
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
        case ACTION_INIT_HW:
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
        switch (action.type) {
        case ACTION_AUDIO_MUTE:
            if (FLAG_SWITCH == action.value) {
                action.value = !audioGet()->par.mute;
            }
            break;
        }
        break;
    }
}

static void ampInitMuteStby(void)
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
    settingsInit();
    ampInitMuteStby();
    rtcInit();

    rtcSetCb(rtcCb);

    utilEnableSwd(SCREEN_STANDBY);

    labelsInit();
    canvasInit();

    mpcInit();

    inputInit(BTN_PLAYER_REWIND | BTN_PLAYER_FORWARD, -2);

    rcInit();

    syncMasterInit();

    ampReadSettings();

    swTimInit();
    tunerSyncInit();

    inputSetPower(false);    // Power off input device

    swTimSet(SW_TIM_RTC_INIT, 500);

    amp.status = AMP_STATUS_STBY;
}

void ampRun(void)
{
    while (1) {
        utilEnableSwd(SCREEN_STANDBY == amp.screen);

        ampGetFromSlaves();

        mpcGetData();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampSendToSlaves();

        ampScreenShow();
    }
}

Amp *ampGet(void)
{
    return &amp;
}

static void ampGetFromSlaves(void)
{
    uint8_t syncData[SYNC_DATASIZE];
    SyncType syncType;
    TunerSync *tunerSync = tunerSyncGet();

    const uint8_t slaves[] = {AMP_TUNER_ADDR, AMP_SPECTRUM_ADDR};

    for (uint8_t i = 0; i < sizeof(slaves); i++) {
        syncType = syncMasterReceive(slaves[i], syncData);
        switch (syncType) {
        case SYNC_ACTION:
            action = *(Action *)&syncData[1];
            return;
        case SYNC_SPECTRUM:
            memcpy(&ampPriv.sp, &syncData[1], sizeof(Spectrum));
            ampPriv.syncFlags |= SYNC_FLAG_SPECTRUM;
            return;
        case SYNC_TUNER_FREQ:
            memcpy(&tunerSync->freq, &syncData[1], sizeof(uint16_t));
            tunerSync->flags |= TUNERSYNC_FLAG_FREQ;
            break;
        case SYNC_TUNER_FLAGS:
            memcpy(&tunerSync->tFlags, &syncData[1], sizeof(TunerFlag));
            tunerSync->flags |= TUNERSYNC_FLAG_FLAGS;
            break;
        case SYNC_TUNER_FAVS:
            memcpy(&tunerSync->favMask, &syncData[1], sizeof(uint16_t));
            tunerSync->flags |= TUNERSYNC_FLAG_FAVS;
            break;
        case SYNC_TUNER_BAND:
            memcpy(&tunerSync->band, &syncData[1], sizeof(TunerSyncBand));
            tunerSync->flags |= TUNERSYNC_FLAG_BAND;
            break;
        }
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

    if (ACTION_NONE == action.type) {
        actionGetPots();
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
        actionRemapBtnShort(action.value);
        break;
    case ACTION_BTN_LONG:
        actionRemapBtnLong(action.value);
        break;
    case ACTION_ENCODER:
        actionRemapEncoder(action.value);
        break;
    case ACTION_REMOTE:
        actionRemapRemote();
        break;

    default:
        break;
    }

    actionRemapCommon();
}

static void ampSendMediaKey(MediaKey key)
{
    InputType inType = amp.inType;

    switch (inType) {
    case IN_MPD:
        mpcSendMediaKey(key);
        break;
    case IN_TUNER:
//        tunerSendMediaKey(key);
//        break;
//    case IN_PC:
//#ifdef _ENABLE_USB
//        usbHidSendMediaKey(key);
//#endif
//        break;
//    case IN_KARADIO:
//        karadioSendMediaKey(key);
//        break;
//    case IN_BLUETOOTH:
//        btSendMediaKey(key);
        break;
    }
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
            ampPriv.syncFlags |= SYNC_FLAG_RTC;
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

    case ACTION_AUDIO_MENU:
        if (scrMode == SCREEN_TUNE) {
            ampPriv.clearScreen = true;
            actionNextAudioParam(aProc);
        } else {
            aProc->tune = AUDIO_TUNE_VOLUME;
        }
        screenSet(SCREEN_TUNE, 5000);
        break;

    case ACTION_AUDIO_SELECT_INPUT:
        ampSetInput(actionGetNextAudioInput((int8_t)action.value));
        screenSet(SCREEN_INPUT, 1000);
        ampPriv.clearScreen = true;
        break;
    case ACTION_AUDIO_SELECT_PARAM:
        audioChangeTune(aProc->tune, (int8_t)action.value);
        if (aProc->tune == AUDIO_TUNE_VOLUME) {
            ampPriv.volume = aProc->par.tune[AUDIO_TUNE_VOLUME].value;
        }
        if (aProc->par.mute) {
            ampMute(false);
        }
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;
    case ACTION_AUDIO_SET_PARAM:
        audioSetTune(aProc->tune, (int8_t)action.value);
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;

    case ACTION_AUDIO_MUTE:
        ampMute(action.value);
        break;

    case ACTION_MEDIA:
        ampSendMediaKey((MediaKey)action.value);
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

static void ampSendToSlaves(void)
{
    if (swTimGet(SW_TIM_SYNC) > 0) {
        return;
    }

    if (ampPriv.syncAction.type != ACTION_NONE) {
        syncMasterSend(AMP_TUNER_ADDR, SYNC_ACTION, &ampPriv.syncAction, sizeof(Action));
        syncMasterSend(AMP_SPECTRUM_ADDR, SYNC_ACTION, &ampPriv.syncAction, sizeof(Action));
        swTimSet(SW_TIM_SYNC, 50);
        // Force everything to resend on exit standby
        if (ampPriv.syncAction.type == ACTION_STANDBY && ampPriv.syncAction.value == FLAG_EXIT) {
            ampPriv.inType = IN_NULL;
        }
        ampPriv.syncAction.type = ACTION_NONE;
        return;
    }

    if (ampPriv.inType != amp.inType) {
        syncMasterSend(AMP_TUNER_ADDR, SYNC_IN_TYPE, &amp.inType, sizeof(InputType));
        syncMasterSend(AMP_SPECTRUM_ADDR, SYNC_IN_TYPE, &amp.inType, sizeof(InputType));
        swTimSet(SW_TIM_SYNC, 50);
        ampPriv.inType = amp.inType;
        return;
    }

    Spectrum *sp = &ampPriv.sp;

    if (ampPriv.syncFlags & SYNC_FLAG_SPECTRUM) {
        ampPriv.syncFlags &= ~SYNC_FLAG_SPECTRUM;
        syncMasterSend(AMP_TUNER_ADDR, SYNC_SPECTRUM, sp, sizeof(Spectrum));
        syncMasterSend(AMP_SPECTRUM_ADDR, SYNC_SPECTRUM, sp, sizeof(Spectrum));
        swTimSet(SW_TIM_SYNC, 50);
        return;
    }

    if (ampPriv.syncFlags & SYNC_FLAG_RTC) {
        ampPriv.syncFlags &= ~SYNC_FLAG_RTC;
        uint32_t rtcRaw = rtcGetRaw();
        syncMasterSend(AMP_TUNER_ADDR, SYNC_TIME, &rtcRaw, sizeof(uint32_t));
        syncMasterSend(AMP_SPECTRUM_ADDR, SYNC_TIME, &rtcRaw, sizeof(uint32_t));
        swTimSet(SW_TIM_SYNC, 50);
        return;
    }
}

static void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    static TuneView tune;

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
        canvasShowInput(clear);
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
}
