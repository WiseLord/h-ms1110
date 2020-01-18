#include "screen.h"

#include "gui/canvas.h"

static bool scrToClear = false;

static Screen screen = {
    .mode = SCREEN_STANDBY,
    .def = SCREEN_SPECTRUM,
};

static bool screenCheckClear(void)
{
    bool clear = false;

    static ScrMode scrPrev = SCREEN_END;

    if (scrToClear) {
        clear = true;
        scrToClear = false;
    } else {
        if (screen.mode != scrPrev) {
            clear = true;
        }
    }

    scrPrev = screen.mode;

    return clear;
}

void screenInit(void)
{
//    labelsInit();
    canvasInit();

//    screenReadSettings();
}

Screen *screenGet(void)
{
    return &screen;
}

void screenSetMode(ScrMode value)
{
    screen.mode = value;
}

void screenToClear(void)
{
    scrToClear = true;
}

void screenShow(void)
{
    bool clear = screenCheckClear();

    if (clear) {
        canvasClear();
    }

    switch (screen.mode) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear);
        break;
    case SCREEN_TIME:
        canvasShowTime(clear);
        break;
    case SCREEN_STANDBY:
        canvasShowStandby(clear);
        break;
    case SCREEN_AUDIO_PARAM:
        canvasShowTune(clear);
        break;
    default:
        break;
    }

    glcdFbSync();
}
