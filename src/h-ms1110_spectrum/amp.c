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
#include "swtimers.h"
#include "sync.h"
#include "timers.h"
#include "tr/labels.h"
#include "utils.h"

static void actionGetButtons(void);
static void actionGetTimers(void);

static void actionRemapSpectrumBtnShort(int16_t button);
static void actionRemapSpectrumBtnLong(int16_t button);

static void ampHandleSwd(void);

static void ampActionSyncMaster(void);

static void ampActionGet(void);
static void ampActionRemap(void);
static void ampActionHandle(void);

static void ampPollInput(void);

static void ampScreenShow(void);

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
    .defScreen = SCREEN_SPECTRUM,
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

    rtcSetMode(RTC_NOEDIT);

    switch (amp.screen) {
    case SCREEN_STANDBY:
        screenSet(SCREEN_STANDBY, 0);
        break;
    default:
        screenSet(amp.defScreen, 0);
        break;
    }
}

static void inputDisable(void)
{
}

static void inputEnable(void)
{
}

static void ampReadSettings(void)
{
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

    settingsStore(PARAM_DISPLAY_DEF, amp.defScreen);

    inputDisable();

    amp.status = AMP_STATUS_STBY;
}

void ampInitHw(void)
{
    swTimSet(SW_TIM_AMP_INIT, SW_TIM_OFF);

    switch (amp.status) {
    case AMP_STATUS_POWERED:
        i2cInit(I2C_AMP, 100000, 0x00);

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
            actionSet(ACTION_SPECTRUM_BTN_LONG, (int16_t)cmdBtn.btn);
        } else {
            actionSet(ACTION_SPECTRUM_BTN_SHORT, (int16_t)cmdBtn.btn);
        }
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
    rtcInit();

    labelsInit();
    canvasInit();

    spInit();

    inputInit();

    syncSlaveInit(AMP_SPECTRUM_ADDR);

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
        amp.clearScreen = true;
        break;
    case SYNC_IN_TYPE:
        amp.inType = *(uint8_t *)&syncData[1];
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
        ScreenType scrMode = amp.screen;

        if (scrMode == SCREEN_STANDBY && rtcCheckAlarm()) {
            actionSet(ACTION_STANDBY, FLAG_EXIT);
        }
    }

    if (ACTION_NONE == action.type) {
        actionGetTimers();
    }
}

static void actionRemapSpectrumBtnShort(int16_t button)
{
    switch (button) {

    }
}

static void actionRemapSpectrumBtnLong(int16_t button)
{
    switch (button) {

    }
}

static void ampActionRemap(void)
{
    switch (action.type) {
    case ACTION_SPECTRUM_BTN_SHORT:
        actionRemapSpectrumBtnShort(action.value);
        break;
    case ACTION_SPECTRUM_BTN_LONG:
        actionRemapSpectrumBtnLong(action.value);
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

            swTimSet(SW_TIM_INPUT_POLL, 200);
        }
    }
}

static void prepareAudioInput (Label *label)
{
    static InputType _inType;

    if (amp.inType != _inType) {
        _inType = amp.inType;
        amp.clearScreen = true;
    }

    *label = LABEL_IN_TUNER + (amp.inType - IN_TUNER);

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

    Label label;

    Spectrum *sp = spGet();

    SpMode spMode = sp->mode == SP_MODE_ANTIMIRROR ? SP_MODE_RIGHT_MIRROR : SP_MODE_RIGHT;

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
    default:
        prepareAudioInput(&label);
        canvasShowInput(clear, 1, label);
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

        // Callbacks
    }
}
