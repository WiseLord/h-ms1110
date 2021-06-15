#include "amp.h"

#include "display/glcd.h"
#include "hwlibs.h"
#include "input.h"
#include "rtc.h"
#include "spectrum.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

static Amp amp = {
    .status = AMP_STATUS_STBY,
    .screen = SCREEN_STANDBY,
};

Amp *ampGet(void)
{
    return &amp;
}

void ampRun(void)
{
    while (1) {
        utilEnableSwd(SCREEN_STANDBY == amp.screen);

        ampSyncFromOthers();

        ampActionGet();
        ampActionRemap();
        ampActionHandle();

        ampSyncToOthers();

        ampScreenShow();
    }
}

Action ampGetButtons()
{
    Action ret = {.type = ACTION_NONE, .value = 0};

    CmdBtn cmdBtn = inputGetBtnCmd();

    if (cmdBtn.btn) {
        ret.value = cmdBtn.btn;
        if (cmdBtn.flags & BTN_FLAG_LONG_PRESS) {
            ret.type = ACTION_BTN_LONG;
        } else {
            ret.type = ACTION_BTN_SHORT;
        }
    }

    return ret;
}

Action ampGetEncoder(void)
{
    Action ret = {.type = ACTION_NONE, .value = 0};

    ret.value = inputGetEncoder();

    if (ret.value) {
        ret.type = ACTION_ENCODER;
    }

    return ret;
}

void ampSetBrightness(uint8_t value)
{
    glcdSetBrightness(value);
}

#ifndef _MODULE_PLAYER

void TIM_SPECTRUM_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_SPECTRUM)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_SPECTRUM);

        // Callbacks
        spConvertADC();
    }
}

#endif

bool ampIsOnline(AmpModule module)
{
    return !!(amp.online & module);
}

void ampSetOffline(AmpModule module)
{
    amp.online &= ~module;
}

void ampSetOnline(AmpModule module)
{
    amp.online |= module;
}

void ampUpdateDate(char *date)
{
    // Parsing date format YYYY-MM-DDTHH:MM:SS
    if (strlen(date) >= 19 &&
        date[4] == '-' &&
        date[7] == '-' &&
        date[10] == 'T' &&
        date[13] == ':' &&
        date[16] == ':') {

        RTC_type rtc;

        rtc.year = strtol(date + 0, NULL, 10) % 100;
        rtc.month = strtol(date + 5, NULL, 10);
        rtc.date = strtol(date + 8, NULL, 10);
        rtc.hour = strtol(date + 11, NULL, 10);
        rtc.min = strtol(date + 14, NULL, 10);
        rtc.sec = strtol(date + 17, NULL, 10);

        rtcUpdateTime(&rtc);
    }
}
