#include "screen.h"

#include "display/glcd.h"
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

    Spectrum *sp = spGet();
    ParamView pview;

    switch (screen.mode) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, sp->mode, sp->peaks);
        break;
    case SCREEN_TIME:
        break;
    case SCREEN_AUDIO_INPUT:
        break;
    case SCREEN_STANDBY:
        break;
    case SCREEN_AUDIO_PARAM:
        pview.val = 20;
        canvasShowParam(clear, &pview);
        break;
    case SCREEN_MENU:
        break;
    default:
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
}
