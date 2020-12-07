#include "inputview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"

static const GlcdRect rectName = {56, 28, 200, 32};

void inputViewDraw(InputView *this, bool clear)
{
    if (!clear) {
        return;
    }

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
