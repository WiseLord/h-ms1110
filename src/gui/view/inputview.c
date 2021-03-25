#include "inputview.h"

#include <stdio.h>

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
    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();

    IconImage iconInputPrev = {
        .rect = &rectIconInputPrev,
        .color = pal->inactive,
        .icon = this->iconPrev,
    };
    iconImageDraw(&iconInputPrev, clear);

    IconImage iconInput = {
        .rect = &rectIconInput,
        .color = pal->fg,
        .icon = this->icon,
    };
    iconImageDraw(&iconInput, clear);

    IconImage iconInputNext = {
        .rect = &rectIconInputNext,
        .color = pal->inactive,
        .icon = this->iconNext,
    };
    iconImageDraw(&iconInputNext, clear);
}
