#include "screen.h"

#include "gui/canvas.h"
#include "tr/labels.h"
#include "settings.h"
#include "spectrum.h"

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

void screenReadSettings(void)
{
    screen.def = (ScrMode)settingsRead(PARAM_DISPLAY_DEF);
}

void screenSaveSettings(void)
{
    settingsStore(PARAM_DISPLAY_DEF, screen.def);
}

void screenInit(void)
{
    labelsInit();
    canvasInit();

    screenReadSettings();
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
        canvasShowDate(clear);
        break;
    case SCREEN_AUDIO_INPUT:
        canvasShowAudioInput(clear);
        break;
    case SCREEN_STANDBY:
        canvasShowStandby(clear);
        break;
    case SCREEN_AUDIO_PARAM:
        canvasShowTune(clear);
        break;
    case SCREEN_MENU:
        canvasShowMenu(clear);
        break;
    default:
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
}
