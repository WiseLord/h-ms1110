#include "setupview.h"

#include "display/glcd.h"
#include "gui/palette.h"
#include "setup.h"
#include "tr/labels.h"

const char *getHeadLabel(SetupType type)
{
    Label label = LABEL_SETUP_MAIN;

    switch (type) {
    case SETUP_TIME:
        label = LABEL_SETUP_TIME;
        break;
    case SETUP_ALARM:
        label = LABEL_SETUP_ALARM;
        break;
    case SETUP_REMOTE:
        label = LABEL_SETUP_REMOTE;
        break;
    }

    return labelsGet(label);
}


static void drawHead(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    static SetupType _old = SETUP_MAIN;
    if (setup->head != _old) {
        _old = setup->head;
        clear = true;
    }

    if (clear) {
        glcdDrawRect(0, 0, rect.w, 32, pal->bg);

        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(0, 0);
        glcdWriteString(getHeadLabel(setup->head));
    }
}

static void drawActive(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    Setup *setup = setupGet();

    static SetupType _old = SETUP_MAIN;
    if (setup->active != _old) {
        _old = setup->active;
        clear = true;
    }

    if (clear) {
        const int16_t marginX = 16;
        glcdDrawRect(marginX, 32, rect.w - marginX * 2, 32, pal->bg);

        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(rect.w / 2, 32);
        glcdSetFontAlign(GLCD_ALIGN_CENTER);
        glcdWriteString(getHeadLabel(setup->active));
    }
}

static void drawArrows(bool clear)
{
    const Palette *pal = paletteGet();
    GlcdRect rect = glcdGet()->rect;

    if (clear) {
        glcdSetFont(&fontterminus32);
        glcdSetFontColor(pal->fg);

        glcdSetXY(0, 32);
        glcdSetFontAlign(GLCD_ALIGN_LEFT);
        glcdWriteString("<");

        glcdSetXY(rect.w, 32);
        glcdSetFontAlign(GLCD_ALIGN_RIGHT);
        glcdWriteString(">");
    }
}


void setupViewDraw(bool clear)
{
    drawHead(clear);
    drawActive(clear);
    drawArrows(clear);
}
