#include "screen.h"

#include "gui/canvas.h"
#include "tr/labels.h"
#include "settings.h"
#include "spectrum.h"
#include "swtimers.h"

static bool scrToClear = false;

static Screen screen = {
    .screen = SCREEN_STANDBY,
    .defScreen = SCREEN_SPECTRUM,
};

static bool screenCheckClear(void)
{
    bool clear = false;

    static ScreenType scrPrev = SCREEN_END;

    if (scrToClear) {
        clear = true;
        scrToClear = false;
    } else {
        if (screen.screen != scrPrev) {
            clear = true;
        }
    }

    scrPrev = screen.screen;

    return clear;
}

void screenReadSettings(void)
{
    screen.defScreen = (ScreenType)settingsRead(PARAM_DISPLAY_DEF);
}

void screenSaveSettings(void)
{
    settingsStore(PARAM_DISPLAY_DEF, screen.defScreen);
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

void screenSetMode(ScreenType value)
{
    screen.screen = value;
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

    switch (screen.screen) {
    case SCREEN_SPECTRUM:
        canvasShowSpectrum(clear, sp->mode, sp->peaks);
        break;
    case SCREEN_TIME:
        break;
    case SCREEN_STANDBY:
        break;
    default:
        break;
    }

    canvasDebugFPS();

    glcdFbSync();
}
