#include "amp.h"

#include <string.h>

#include "audio/audio.h"
#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input/analog.h"
#include "input.h"
#include "mediakey.h"
#include "mpc.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "setup.h"
#include "spectrum.h"
#include "swtimers.h"
#include "sync.h"
#include "tunersync.h"
#include "utils.h"

#define SYNC_PERIOD     10

typedef uint8_t SyncFlags;
enum {
    SYNC_FLAG_NONE      = 0x00,

    SYNC_FLAG_RTC       = 0x01,
    SYNC_FLAG_SPECTRUM  = 0x02,
    SYNC_FLAG_REQUEST   = 0x04,
    SYNC_FLAG_IN_TYPE   = 0x08,

    SYNC_FLAG_ALL       = 0x0F,
};

typedef struct {
    SyncFlags syncFlags;

    bool clearScreen;

    ScreenType prevScreen;

    uint8_t inputStatus;
    int8_t volume;
    uint8_t silenceTimer;

    Spectrum sp;
    Action syncAction;

    AudioTune tune;

    int8_t inIdx;
    bool inIdxInc;
} AmpPriv;

static void actionGetRemote(void);
static void actionGetPots(void);
static void actionGetTimers(void);

static void actionRemapBtnShort(int16_t button);
static void actionRemapBtnLong(int16_t button);
static void actionRemapRemote(void);
static void actionRemapEncoder(int16_t encCnt);

static void actionRemapCommon(void);

static void ampPingSlaves(void);
static void ampGetFromSlaves(void);
static void ampSendToSlaves(void);

static void sendToTunerModule(SyncType type, void *data, size_t size);
static void sendToSpectrumModule(SyncType type, void *data, size_t size);

static int8_t actionGetNextAudioInput(int8_t diff);

static AmpPriv priv;
static Amp *amp;

static const InputPair inPairs[] = {
    {0, IN_TUNER},
    {1, IN_MPD},
    {2, IN_SPDIF},
    {4, IN_AUX1},
    {5, IN_AUX2},
};

static const InputMap inMap = {
    .pairs = inPairs,
    .mapSize = sizeof(inPairs) / sizeof(InputPair),
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
    priv.syncFlags |= SYNC_FLAG_RTC;
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
    priv.tune = AUDIO_TUNE_VOLUME;

    switch (amp->screen) {
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
    if (priv.silenceTimer) {
        swTimSet(SW_TIM_SILENCE, 1000 * 60 * priv.silenceTimer + 999);
    }
}

static void inputDisable(void)
{
    // TODO: Power off current input device
}

static void inputEnable(void)
{
    InputType inType = amp->inType;

    switch (inType) {
    case IN_TUNER:
        priv.syncFlags |= SYNC_FLAG_REQUEST;
        break;
    case IN_MPD:
        mpcSyncRequest();
        break;
    }

    // TODO: Power on current input device
}

static void inputSetPower(bool value)
{
    AudioProc *aProc = audioGet();
    int8_t input = aProc->par.input;

    if (value) {
        priv.inputStatus = (uint8_t)(1 << input);
    } else {
        priv.inputStatus = 0x00;
    }

    for (uint8_t i = 0; i < inMap.mapSize; i++) {
        if (inPairs[i].input == input) {
            amp->inType = inPairs[i].type;
            priv.inIdx = i;
            break;
        }
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
    const AudioGrid *grid = aProc->par.grid[AUDIO_TUNE_VOLUME];

    if (value) {
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
    } else {
        audioSetTune(AUDIO_TUNE_VOLUME, grid->min);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_ON);
    }

    ampPinMute(value);
    audioSetFlag(AUDIO_FLAG_MUTE, value);
}

static void ampReadSettings(void)
{
    audioReadSettings(AUDIO_IC_TDA7719);
}

static void ampVolumeInit(void)
{
    AudioProc *aProc = audioGet();
    const AudioGrid *grid = aProc->par.grid[AUDIO_TUNE_VOLUME];
    int8_t *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];

    priv.volume = *volItem;
    *volItem = grid->min;
}

void ampExitStby(void)
{
    swTimSet(SW_TIM_MPD_POWEROFF, SW_TIM_OFF);

    ampSetBrightness(AMP_BR_ACTIVE);

    ampReadSettings();

    ampPinStby(false);      // Power on amplifier

    inputSetPower(true);    // Power on input device

    amp->status = AMP_STATUS_POWERED;

    swTimSet(SW_TIM_AMP_INIT, 200);
}

void ampEnterStby(void)
{
    swTimSet(SW_TIM_MPD_POWEROFF, 20000); // Turn off MPD after 60 seconds
    mpcSendMediaKey(MEDIAKEY_STOP);

    ampSetBrightness(AMP_BR_STBY);

    swTimSet(SW_TIM_STBY, SW_TIM_OFF);
    swTimSet(SW_TIM_SILENCE, SW_TIM_OFF);

    ampMute(true);

    // Restore volume value before saving
    AudioProc *aProc = audioGet();
    int8_t *volItem = &aProc->par.tune[AUDIO_TUNE_VOLUME];
    *volItem = priv.volume;

    audioSetPower(false);

    inputDisable();

    inputSetPower(false);   // Power off input device

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
        priv.syncAction = action;    // Send to slaves
        ampEnterStby();
        screenSet(SCREEN_STANDBY, 0);
        break;
    case FLAG_EXIT:
        priv.syncFlags |= SYNC_FLAG_ALL;
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

        audioInit();
        ampVolumeInit();

        audioSetPower(true);
        ampMute(true);

        rdsParserReset();

        swTimSet(SW_TIM_AMP_INIT, 300);
        amp->status = AMP_STATUS_HW_READY;
        break;
    case AMP_STATUS_HW_READY:
        inputEnable();

        ampMute(false);

        swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
        amp->status = AMP_STATUS_ACTIVE;
        break;
    case AMP_STATUS_INACTIVE:
        ampPinStby(true);

        swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);
        amp->status = AMP_STATUS_STBY;
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

    amp->status = AMP_STATUS_HW_READY;
    swTimSet(SW_TIM_AMP_INIT, 300);
}

static void actionNextAudioGroup(void)
{
    AudioGroup group = audioGetGroup(priv.tune);

    do {
        group = (group + 1) % AUDIO_GROUP_END;
        priv.tune = audioGetFirstInGroup(group);
    } while (priv.tune == AUDIO_TUNE_INVALID);
}

static void actionNextAudioSubParam(void)
{
    AudioGroup group = audioGetGroup(priv.tune);

    if (group == AUDIO_GROUP_INVALID || priv.tune == AUDIO_TUNE_INVALID) {
        return;
    }

    do {
        priv.tune++;
        if (priv.tune >= AUDIO_TUNE_END) {
            priv.tune = AUDIO_TUNE_VOLUME;
        }
    } while (!audioIsTuneValid(priv.tune) || audioGetGroup(priv.tune) != group);
}

static int8_t actionGetNextAudioInput(int8_t diff)
{
    priv.inIdx = (inMap.mapSize + priv.inIdx + diff) % inMap.mapSize;

    return inPairs[priv.inIdx].input;
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

static bool isRemoteCmdRepeatable(RcCmd cmd)
{
    ScreenType scrMode = amp->screen;

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
    const AudioGrid *grid = aProc->par.grid[AUDIO_TUNE_BASS];
    uint16_t zoneCnt = grid->max - grid->min + 1;

    for (AinChannel ain = AIN_POT_A; ain < AIN_POT_END; ain++) {
        int8_t pot = inputAnalogGetPotZone(ain, zoneCnt);
        if (pot != potPrev[ain]) {
            if (amp->status == AMP_STATUS_ACTIVE) {
                amp->screen = SCREEN_TUNE;
                switch (ain) {
                case  AIN_POT_A:
                    if (priv.tune != AUDIO_TUNE_BASS) {
                        priv.tune = AUDIO_TUNE_BASS;
                        priv.clearScreen = true;
                    }
                    actionSet(ACTION_AUDIO_SET_PARAM, pot + grid->min);
                    break;
                case AIN_POT_B:
                    if (priv.tune != AUDIO_TUNE_TREBLE) {
                        priv.tune = AUDIO_TUNE_TREBLE;
                        priv.clearScreen = true;
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
    } else if (swTimGet(SW_TIM_STBY) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_MPD_POWEROFF) == 0) {
        actionSet(ACTION_MPD_POWEROFF, 0);
    } else if (swTimGet(SW_TIM_SILENCE) == 0) {
        actionSet(ACTION_STANDBY, FLAG_ENTER);
    } else if (swTimGet(SW_TIM_RTC_INIT) == 0) {
        actionSet(ACTION_INIT_RTC, 0);
    } else if (swTimGet(SW_TIM_SOFT_VOLUME) == 0) {
        actionSet(ACTION_RESTORE_VOLUME, priv.volume);
    } else if (swTimGet(SW_TIM_DISPLAY) == 0) {
        actionSet(ACTION_DISP_EXPIRED, 0);
    }
}

static void actionRemapBtnShort(int16_t button)
{
    Setup *setup = setupGet();

    switch (button) {
    case BTN_AMP_STBY:
        switch (amp->screen) {
        case SCREEN_SETUP:
            if (setup->active <= SETUP_MAIN) { // has childs
                actionSet(ACTION_SETUP_SELECT, setup->child);
            }
            break;
        default:
            actionSet(ACTION_STANDBY, FLAG_SWITCH);
        }
        break;
    case BTN_AMP_IN_PREV:
        if (SCREEN_SETUP == amp->screen) {
            actionSet(ACTION_SETUP_SWITCH_CHILD, -1);
        } else {
            actionSet(ACTION_AUDIO_SELECT_INPUT, -1);
        }
        break;
    case BTN_AMP_IN_NEXT:
        if (SCREEN_SETUP == amp->screen) {
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
        actionSet(ACTION_AUDIO_MENU, 0);
        break;
    case BTN_PLAYER_SUBTITLE:
        actionSet(ACTION_AUDIO_MENU, 1);
        break;
    default:
        break;
    }
}

static void actionRemapBtnLong(int16_t button)
{
    Setup *setup = setupGet();

    switch (button) {
    case BTN_AMP_STBY:
        switch (amp->screen) {
        case SCREEN_STANDBY:
            actionSet(ACTION_SETUP_SELECT, SETUP_MAIN);
            break;
        case SCREEN_SETUP:
            actionSet(ACTION_STANDBY, FLAG_ENTER);
            if (setup->active <= SETUP_MAIN) { // has childs
                actionSet(ACTION_STANDBY, FLAG_ENTER);
            } else {
                actionSet(ACTION_SETUP_BACK, 0);
            }
            break;
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
    ScreenType scrMode = amp->screen;

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
    case RC_CMD_MUTE:
        actionSet(ACTION_AUDIO_MUTE, FLAG_SWITCH);
        break;
    case RC_CMD_IN_NEXT:
        actionSet(ACTION_AUDIO_SELECT_INPUT, +1);
        break;
    case RC_CMD_VOL_UP:
        amp->screen = SCREEN_TUNE;
        actionSet(ACTION_AUDIO_SELECT_PARAM, +1);
        break;
    case RC_CMD_VOL_DOWN:
        amp->screen = SCREEN_TUNE;
        actionSet(ACTION_AUDIO_SELECT_PARAM, -1);
        break;
    case RC_CMD_CHAN_PREV:
        actionSet(ACTION_MEDIA, MEDIAKEY_PREV);
        break;
    case RC_CMD_CHAN_NEXT:
        actionSet(ACTION_MEDIA, MEDIAKEY_NEXT);
        break;
    case RC_CMD_AUDIO_MENU:
        actionSet(ACTION_AUDIO_MENU, 0);
        break;
    case RC_CMD_AUDIO_SUBMENU:
        actionSet(ACTION_AUDIO_MENU, 1);
        break;
    case RC_CMD_DIG_0:
    case RC_CMD_DIG_1:
    case RC_CMD_DIG_2:
    case RC_CMD_DIG_3:
    case RC_CMD_DIG_4:
    case RC_CMD_DIG_5:
    case RC_CMD_DIG_6:
    case RC_CMD_DIG_7:
    case RC_CMD_DIG_8:
    case RC_CMD_DIG_9:
        break;
    case RC_CMD_NAV_LEFT:
        actionSet(ACTION_MEDIA, MEDIAKEY_REWIND);
        break;
    case RC_CMD_NAV_RIGHT:
        actionSet(ACTION_MEDIA, MEDIAKEY_FFWD);
        break;
    case RC_CMD_NAV_UP:
        actionSet(ACTION_MEDIA, MEDIAKEY_NEXT);
        break;
    case RC_CMD_NAV_DOWN:
        actionSet(ACTION_MEDIA, MEDIAKEY_PREV);
        break;
    case RC_CMD_NAV_OK:
        break;
    case RC_CMD_NAV_BACK:
        break;
    case RC_CMD_TIME:
        break;
    case RC_CMD_PLAY:
        actionSet(ACTION_MEDIA, MEDIAKEY_PLAY);
        break;
    case RC_CMD_PAUSE:
        actionSet(ACTION_MEDIA, MEDIAKEY_PAUSE);
        break;
    case RC_CMD_STOP:
        actionSet(ACTION_MEDIA, MEDIAKEY_STOP);
        break;
    case RC_CMD_REW:
        actionSet(ACTION_MEDIA, MEDIAKEY_REWIND);
        break;
    case RC_CMD_FWD:
        actionSet(ACTION_MEDIA, MEDIAKEY_FFWD);
        break;
    case RC_CMD_PREV:
        actionSet(ACTION_MEDIA, MEDIAKEY_PREV);
        break;
    case RC_CMD_NEXT:
        actionSet(ACTION_MEDIA, MEDIAKEY_NEXT);
        break;
    case RC_CMD_SP_MODE:
        priv.syncAction.type = ACTION_SP_CHANGE_MODE;
        priv.syncAction.value = +1;
        break;
    case RC_CMD_SP_PEAKS:
        priv.syncAction.type = ACTION_SP_CHANGE_PEAKS;
        priv.syncAction.value = 0;
        break;
    case RC_CMD_SP_DEMO:
        priv.syncAction.type = ACTION_SP_CHANGE_DEMO;
        priv.syncAction.value = 0;
        break;
    default:
        break;
    }
}

static void actionRemapEncoder(int16_t encCnt)
{
    ScreenType scrMode = amp->screen;

    if (SCREEN_STANDBY == scrMode) {
        return;
    }

    switch (scrMode) {
    case SCREEN_SETUP:
        actionSet(ACTION_SETUP_CHANGE_CHILD, encCnt);
        break;
    default:
        if (priv.tune == AUDIO_TUNE_BASS || priv.tune == AUDIO_TUNE_TREBLE) {
            priv.clearScreen = true;
            priv.tune = AUDIO_TUNE_VOLUME;
        }
        actionSet(ACTION_AUDIO_SELECT_PARAM, encCnt);
        break;
    }
}

static void actionRemapCommon(void)
{
    switch (amp->screen) {
    case SCREEN_STANDBY:
        switch (action.type) {
        case ACTION_STANDBY:
            if (FLAG_SWITCH == action.value) {
                switch (amp->status) {
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
        case ACTION_MPD_POWEROFF:
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
                action.value = !(audioGet()->par.flags & AUDIO_FLAG_MUTE);
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
    amp = ampGet();

    settingsInit();
    utilInitSysCounter();

    ampInitMuteStby();

    rtcInitAlarms();

    labelsInit();
    canvasInit();

    ampSetBrightness(AMP_BR_STBY);

    rtcInit();
    rtcSetCb(rtcCb);

    inputInit(BTN_PLAYER_REWIND | BTN_PLAYER_FORWARD, -2);

    rcInit();

    syncMasterInit();

    ampReadSettings();

    swTimInit();

    mpcInit();
    tunerSyncInit();

    inputSetPower(false);    // Power off input device

    swTimSet(SW_TIM_RTC_INIT, 500);

    amp->status = AMP_STATUS_STBY;
    ampSetOffline(AMP_MODULE_TUNER);
    ampSetOffline(AMP_MODULE_SPECTRUM);
}

void ampSyncFromOthers(void)
{
    ampGetFromSlaves();
    mpcGetData();
}

void ampSyncToOthers(void)
{
    ampSendToSlaves();
}

static bool receiveFromTunerModule(void)
{
    bool ret = false;

    uint8_t syncData[SYNC_DATASIZE];
    SyncType syncType;

    TunerSync *tunerSync = tunerSyncGet();
    RdsParser *rdsParser = rdsParserGet();

    syncType = syncMasterReceive(AMP_TUNER_ADDR, syncData);
    switch (syncType) {
    case SYNC_ERR:
        // Communication error
        ampSetOffline(AMP_MODULE_TUNER);
        break;
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_TUNER_FREQ:
        memcpy(&tunerSync->freq, &syncData[1], sizeof(uint16_t));
        tunerSync->flags |= TUNERSYNC_FLAG_FREQ;
        break;
    case SYNC_TUNER_STNUM:
        memcpy(&tunerSync->stNum, &syncData[1], sizeof(int8_t));
        tunerSync->flags |= TUNERSYNC_FLAG_STNUM;
        break;
    case SYNC_TUNER_STATUS:
        memcpy(&tunerSync->statusFlags, &syncData[1], sizeof(TunerStatusFlag));
        tunerSync->flags |= TUNERSYNC_FLAG_STATUS;
        break;
    case SYNC_TUNER_PARAM:
        memcpy(&tunerSync->paramFlags, &syncData[1], sizeof(TunerParamFlag));
        tunerSync->flags |= TUNERSYNC_FLAG_PARAM;
        break;
    case SYNC_TUNER_FAVS:
        memcpy(&tunerSync->favMask, &syncData[1], sizeof(uint16_t));
        tunerSync->flags |= TUNERSYNC_FLAG_FAVS;
        break;
    case SYNC_TUNER_BAND:
        memcpy(&tunerSync->band, &syncData[1], sizeof(TunerSyncBand));
        tunerSync->flags |= TUNERSYNC_FLAG_BAND;
        break;
    case SYNC_TUNER_RDS:
        memcpy(rdsParser, &syncData[1], sizeof(RdsParser));
        tunerSync->flags |= TUNERSYNC_FLAG_RDS;
        break;
    }

    return ret;
}

static void receiveFromSpectrumModule(void)
{
    uint8_t syncData[SYNC_DATASIZE];
    SyncType syncType;

    syncType = syncMasterReceive(AMP_SPECTRUM_ADDR, syncData);
    switch (syncType) {
    case SYNC_ERR:
        // Communication error
        ampSetOffline(AMP_MODULE_SPECTRUM);
        break;
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_SPECTRUM:
        memcpy(&priv.sp, &syncData[1], sizeof(Spectrum));
        priv.syncFlags |= SYNC_FLAG_SPECTRUM;
        break;
    }
}

static void ampPingSlaves(void)
{
    if (!ampIsOnline(AMP_MODULE_TUNER)) {
        SyncType syncType = syncMasterSend(AMP_TUNER_ADDR, SYNC_PING, NULL, 0);
        if (syncType != SYNC_ERR) {
            ampSetOnline(AMP_MODULE_TUNER);
            priv.syncFlags = SYNC_FLAG_ALL;
            swTimSet(SW_TIM_SYNC, 100);
        }
    }

    if (!ampIsOnline(AMP_MODULE_SPECTRUM)) {
        SyncType syncType = syncMasterSend(AMP_SPECTRUM_ADDR, SYNC_PING, NULL, 0);
        if (syncType != SYNC_ERR) {
            ampSetOnline(AMP_MODULE_SPECTRUM);
            priv.syncFlags = SYNC_FLAG_ALL;
            swTimSet(SW_TIM_SYNC, 100);
        }
    }
}

static void ampGetFromSlaves(void)
{
    if (ampIsOnline(AMP_MODULE_TUNER)) {
        receiveFromTunerModule();
    }
    if (ampIsOnline(AMP_MODULE_SPECTRUM)) {
        receiveFromSpectrumModule();
    }
}

void ampActionGet(void)
{
    if (ACTION_NONE == action.type) {
        action = ampGetButtons();
    }

    if (ACTION_NONE == action.type) {
        action = ampGetEncoder();
    }

    if (ACTION_NONE == action.type) {
        actionGetRemote();
    }

    if (ACTION_NONE == action.type) {
        actionGetPots();
    }

    if (ACTION_NONE == action.type) {
        ScreenType scrMode = amp->screen;

        if (scrMode == SCREEN_STANDBY && rtcCheckAlarm()) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
        }
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
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
    InputType inType = amp->inType;

    switch (inType) {
    case IN_MPD:
        mpcSendMediaKey(key);
        break;
    case IN_TUNER:
        priv.syncAction = action;
        break;
    default:
        break;
    }
}

void ampActionHandle(void)
{
    ScreenType scrMode = amp->screen;

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
        if (aProc->par.tune[AUDIO_TUNE_VOLUME] < action.value) {
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
            priv.syncFlags |= SYNC_FLAG_RTC;
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
            priv.clearScreen = true;
            if (action.value == 0) {
                actionNextAudioGroup();
            } else {
                actionNextAudioSubParam();
            }
        }
        screenSet(SCREEN_TUNE, 5000);
        break;

    case ACTION_AUDIO_SELECT_INPUT:
        if (scrMode == SCREEN_INPUT_SELECTOR) {
            ampSetInput(actionGetNextAudioInput((int8_t)action.value));
            swTimSet(SW_TIM_SCROLL, 120);
            priv.inIdxInc = (action.value > 0);
        } else {
            priv.clearScreen = true;
            swTimSet(SW_TIM_SCROLL, SW_TIM_OFF);
        }
        screenSet(SCREEN_INPUT_SELECTOR, 2000);
        priv.syncFlags |= SYNC_FLAG_IN_TYPE;
        break;
    case ACTION_AUDIO_SELECT_PARAM:
        audioChangeTune(priv.tune, (int8_t)action.value);
        if (priv.tune == AUDIO_TUNE_VOLUME) {
            priv.volume = aProc->par.tune[AUDIO_TUNE_VOLUME];
        }
        if (aProc->par.flags & AUDIO_FLAG_MUTE) {
            ampMute(false);
        }
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;
    case ACTION_AUDIO_SET_PARAM:
        audioSetTune(priv.tune, (int8_t)action.value);
        screenSet(SCREEN_TUNE, 3000);
        swTimSet(SW_TIM_SOFT_VOLUME, SW_TIM_OFF);
        break;

    case ACTION_AUDIO_MUTE:
        ampMute(action.value);
        priv.clearScreen = true;
        actionDispExpired();
        break;

    case ACTION_DIGIT:
        if (amp->inType == IN_MPD) {
            mpcLoadPlaylist((uint8_t)action.value);
        }
        break;

    case ACTION_MEDIA:
        ampSendMediaKey((MediaKey)action.value);
        break;

    case ACTION_MPD_POWEROFF:
        mpcSchedPower(false);
        swTimSet(SW_TIM_MPD_POWEROFF, SW_TIM_OFF);
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

static void sendToTunerModule(SyncType type, void *data, size_t size)
{
    if (ampIsOnline(AMP_MODULE_TUNER)) {
        SyncType syncType = syncMasterSend(AMP_TUNER_ADDR, type, data, size);
        if (syncType == SYNC_ERR) {
            ampSetOffline(AMP_MODULE_TUNER);
        }
    }
}

static void sendToSpectrumModule(SyncType type, void *data, size_t size)
{
    if (ampIsOnline(AMP_MODULE_SPECTRUM)) {
        SyncType syncType = syncMasterSend(AMP_SPECTRUM_ADDR, type, data, size);
        if (syncType == SYNC_ERR) {
            ampSetOffline(AMP_MODULE_SPECTRUM);
        }
    }
}

static void sendToAllModules(SyncType type, void *data, size_t size)
{
    sendToTunerModule(type, data, size);
    sendToSpectrumModule(type, data, size);
}

static void ampSendToSlaves(void)
{
    if (swTimGet(SW_TIM_SYNC) > 0) {
        return;
    }

    if (priv.syncAction.type != ACTION_NONE) {
        sendToAllModules(SYNC_ACTION, &priv.syncAction, sizeof(Action));
        swTimSet(SW_TIM_SYNC, SYNC_PERIOD);
        priv.syncAction.type = ACTION_NONE;
        return;
    }

    if (priv.syncFlags & SYNC_FLAG_IN_TYPE) {
        priv.syncFlags &= ~SYNC_FLAG_IN_TYPE;
        sendToAllModules(SYNC_IN_TYPE, &amp->inType, sizeof(InputType));
        swTimSet(SW_TIM_SYNC, SYNC_PERIOD);
        return;
    }

    if (priv.syncFlags & SYNC_FLAG_RTC) {
        priv.syncFlags &= ~SYNC_FLAG_RTC;
        uint32_t rtcRaw = rtcGetRaw();
        sendToAllModules(SYNC_TIME, &rtcRaw, sizeof(uint32_t));
        swTimSet(SW_TIM_SYNC, SYNC_PERIOD);
        ampPingSlaves();
        return;
    }

    Spectrum *sp = &priv.sp;

    if (priv.syncFlags & SYNC_FLAG_SPECTRUM) {
        priv.syncFlags &= ~SYNC_FLAG_SPECTRUM;
        sendToTunerModule(SYNC_SPECTRUM, sp, sizeof(Spectrum));
        swTimSet(SW_TIM_SYNC, SYNC_PERIOD);
        return;
    }

    if (priv.syncFlags & SYNC_FLAG_REQUEST) {
        priv.syncFlags &= ~SYNC_FLAG_REQUEST;
        sendToAllModules(SYNC_REQUEST, NULL, 0);
        swTimSet(SW_TIM_SYNC, SYNC_PERIOD);
        return;
    }
}

void ampScreenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
        swTimSet(SW_TIM_SCROLL, SW_TIM_OFF);
    }

    DateTimeMode dtMode = DT_MODE_TIME | DT_MODE_DATE | DT_MODE_WDAY;
    if (ampIsOnline(AMP_MODULE_TUNER | AMP_MODULE_SPECTRUM)) {
        dtMode &= ~(DT_MODE_DATE | DT_MODE_WDAY);
    }

    switch (amp->screen) {
    case SCREEN_TIME:
        canvasShowDateTime(clear, dtMode);
        break;
    case SCREEN_STANDBY:
        canvasShowDateTime(clear, dtMode);
        break;
    case SCREEN_TUNE:
        canvasShowTune(clear, priv.tune);
        break;
    case SCREEN_SETUP:
        canvasShowSetup(clear);
        break;
    case SCREEN_INPUT_SELECTOR:
        canvasShowInputSelector(clear, priv.inIdx, priv.inIdxInc, &inMap);
        break;
    default:
        canvasShowInput(clear, amp->inType);
        break;
    }

    canvasDebugFPS();

    glcdSync();
}
