#include "amp.h"

#include "gui/canvas.h"
#include "pins.h"

static Action action = {
    .type = ACTION_INIT,
    .screen = SCREEN_STANDBY,
};

static Screen screen = {
    .mode = SCREEN_STANDBY,
    .def = SCREEN_STANDBY,
};

void ampPinMute(bool value)
{
    if (value) {
        SET(MUTE);
    } else {
        CLR(MUTE);
    }
}

void ampPinStby(bool value)
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
}


void ampActionGet(void)
{

}

void ampActionHandle(void)
{
    action.timeout = 0;

    switch (action.type) {
    case ACTION_INIT:
        ampInit();
        break;
    default:
        break;
    }

}

void ampShowScreen()
{
    switch (screen.mode) {
    case SCREEN_STANDBY:
        canvasShowStandby(false);
        break;
    default:
        break;
    }

    glcdFbSync();
}
