#include "amp.h"

#include "display/glcd.h"
#include "input.h"
#include "utils.h"

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
