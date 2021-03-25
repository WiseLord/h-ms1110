#include "inputview.h"

#include <stdio.h>

#include "amp.h"
#include "display/glcd.h"
#include "gui/palette.h"
#include "gui/widget/iconimage.h"

static const GlcdRect rectName = {56, 4, 200, 32};

static const GlcdRect rectIconInputPrev = {178, 0, 40, 40};
static const GlcdRect rectIconInput = {108, 0, 40, 40};
static const GlcdRect rectIconInputNext = {38, 0, 40, 40};

void inputViewDrawName(InputView *this)
{
    const Palette *pal = paletteGet();
    const GlcdRect *rect = &rectName;

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    char buf[32];
    snprintf(buf, sizeof(buf), "%s", this->name);

    glcdSetRect(rect);

    glcdSetXY(0, 0);
    glcdWriteString(buf);

    glcdResetRect();

}

void inputViewDraw(InputView *this, bool clear)
{
    if (this->scrollTimer > 0) {
        clear = true;
    }

    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    const InputMap *inMap = this->inMap;
    const int8_t inIdx = this->inIdx;

    IconImage iconInputPrev = {
        .rect = &rectIconInputPrev,
        .color = COLOR_GRAY16(1),
        .icon = ICON_TUNER + (inMap->pairs[(inMap->mapSize + inIdx - 1) % inMap->mapSize].type - IN_TUNER),
    };
    iconImageDraw(&iconInputPrev, clear);

    IconImage iconInput = {
        .rect = &rectIconInput,
        .color = pal->fg,
        .icon = ICON_TUNER + (inMap->pairs[inIdx].type - IN_TUNER),
    };
    iconImageDraw(&iconInput, clear);

    IconImage iconInputNext = {
        .rect = &rectIconInputNext,
        .color = COLOR_GRAY16(1),
        .icon = ICON_TUNER + (inMap->pairs[(inMap->mapSize + inIdx + 1) % inMap->mapSize].type - IN_TUNER),
    };
    iconImageDraw(&iconInputNext, clear);
}
