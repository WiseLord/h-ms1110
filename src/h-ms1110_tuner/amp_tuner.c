#include "amp.h"

#include <stddef.h>
#include <string.h>

#include "debug.h"
#include "gui/canvas.h"
#include "hwlibs.h"
#include "i2c.h"
#include "input.h"
#include "rc.h"
#include "rtc.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"
#include "sync.h"
#include "timers.h"
#include "tr/labels.h"
#include "tuner/stations.h"
#include "tuner/tuner.h"
#include "utils.h"

typedef struct {
    ScreenType prevScreen;
    bool clearScreen;
} AmpPriv;

static void actionGetButtons(void);
static void actionGetEncoder(void);
static void actionGetTimers(void);

static void actionRemapTunerBtnShort(int16_t button);
static void actionRemapTunerBtnLong(int16_t button);
static void actionRemapTunerEncoder(int16_t encCnt);

static void ampActionSyncMaster(void);

static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampPollInput(void);

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
    ScreenType defScreen = SCREEN_SPECTRUM;

    switch (amp.inType) {
    case IN_TUNER:
        defScreen = SCREEN_TUNER;
        break;
    }

    rtcSetMode(RTC_NOEDIT);

    switch (amp.screen) {
    case SCREEN_STANDBY:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(defScreen, 0);
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

        tunerInit();

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
            actionSet(ACTION_TUNER_BTN_LONG, (int16_t)cmdBtn.btn);
        } else {
            actionSet(ACTION_TUNER_BTN_SHORT, (int16_t)cmdBtn.btn);
        }
        syncSlaveSendAction(&action);
    }
}

static void actionGetEncoder(void)
{
    int8_t encVal = inputGetEncoder();

    if (encVal) {
        actionSet(ACTION_TUNER_ENCODER, encVal);
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
        ampHandleSwd(amp.screen);

        ampActionSyncMaster();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampPollInput();

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

    Spectrum *sp = spGet();

    switch (syncType) {
    case SYNC_ACTION:
        action = *(Action *)&syncData[1];
        break;
    case SYNC_TIME:
        rtcSetRaw(*(uint32_t *)&syncData[1]);
        break;
    case SYNC_SPECTRUM:
        *sp = *((Spectrum *)&syncData[1]);
        settingsStore(PARAM_SPECTRUM_MODE, sp->mode);
        settingsStore(PARAM_SPECTRUM_PEAKS, sp->peaks);
        ampPriv.clearScreen = true;
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

static void actionRemapTunerBtnShort(int16_t button)
{
    switch (button) {
    case BTN_TUNER_1:
        actionSet(ACTION_DIGIT, 1);
        break;
    case BTN_TUNER_2:
        actionSet(ACTION_DIGIT, 2);
        break;
    case BTN_TUNER_3:
        actionSet(ACTION_DIGIT, 3);
        break;
    case BTN_TUNER_4:
        actionSet(ACTION_DIGIT, 4);
        break;
    case BTN_TUNER_5:
        actionSet(ACTION_DIGIT, 5);
        break;
    case BTN_TUNER_6:
        actionSet(ACTION_DIGIT, 6);
        break;
    case BTN_TUNER_7:
        actionSet(ACTION_DIGIT, 7);
        break;
    case BTN_TUNER_8:
        actionSet(ACTION_DIGIT, 8);
        break;
    case BTN_TUNER_9:
        actionSet(ACTION_DIGIT, 9);
        break;
    case BTN_TUNER_MWFM:
        break;
    case BTN_TUNER_RDS:
        break;
    case BTN_TUNER_ENC:
        break;
    default:
        break;
    }
}

static void actionRemapTunerBtnLong(int16_t button)
{
    Tuner *tuner = tunerGet();

    switch (button) {
    case BTN_TUNER_1:
        actionSet(ACTION_DIGIT_HOLD, 1);
        break;
    case BTN_TUNER_2:
        actionSet(ACTION_DIGIT_HOLD, 2);
        break;
    case BTN_TUNER_3:
        actionSet(ACTION_DIGIT_HOLD, 3);
        break;
    case BTN_TUNER_4:
        actionSet(ACTION_DIGIT_HOLD, 4);
        break;
    case BTN_TUNER_5:
        actionSet(ACTION_DIGIT_HOLD, 5);
        break;
    case BTN_TUNER_6:
        actionSet(ACTION_DIGIT_HOLD, 6);
        break;
    case BTN_TUNER_7:
        actionSet(ACTION_DIGIT_HOLD, 7);
        break;
    case BTN_TUNER_8:
        actionSet(ACTION_DIGIT_HOLD, 8);
        break;
    case BTN_TUNER_9:
        actionSet(ACTION_DIGIT_HOLD, 9);
        break;
    case BTN_TUNER_MWFM:
        break;
    case BTN_TUNER_RDS:
        break;
    case BTN_TUNER_ENC:
        stationStore(tuner->status.freq, "test");
        break;
    default:
        break;
    }
}

static void actionRemapTunerEncoder(int16_t encCnt)
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
    case ACTION_TUNER_BTN_SHORT:
        actionRemapTunerBtnShort(action.value);
        break;
    case ACTION_TUNER_BTN_LONG:
        actionRemapTunerBtnLong(action.value);
        break;
    case ACTION_TUNER_ENCODER:
        actionRemapTunerEncoder(action.value);
        break;
    }

    switch (action.type) {
    case ACTION_DIGIT:
        if (amp.inType == IN_TUNER) {
            stationFavZap(action.value);
        }
        break;
    case ACTION_DIGIT_HOLD:
        if (amp.inType == IN_TUNER) {
            stationFavStoreRemove(action.value);
        }
        break;
    }
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

static void ampPollInput(void)
{
    if (amp.screen != SCREEN_STANDBY) {
        if (swTimGet(SW_TIM_INPUT_POLL) == 0) {
            if (amp.inType == IN_TUNER) {
                tunerUpdateStatus();
            }
            swTimSet(SW_TIM_INPUT_POLL, 200);
        }
    }
}

static void prepareRadioView(RadioView *radio)
{
    Tuner *tuner = tunerGet();

    radio->freq = tuner->status.freq;
    radio->stereo = ((tuner->status.flags & TUNER_FLAG_STEREO) == TUNER_FLAG_STEREO);
    radio->station = ((tuner->status.flags & TUNER_FLAG_STATION) == TUNER_FLAG_STATION);
    radio->stationNum = stationGetNum(tuner->status.freq);
    radio->favMask = stationFavGetMask(radio->freq);
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
