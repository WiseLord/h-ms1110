#include "mpcview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"

void mpcViewDraw(bool clear, MpcView *view)
{
    (void)clear;

    const Palette *pal = paletteGet();
    const tFont *font = &fontterminus20b;
    GlcdRect rect = glcdGet()->rect;

    glcdSetFont(font);
    glcdSetFontColor(pal->active);

    char buf[64];

    snprintf(buf, sizeof(buf), "%s", view->name);
    glcdSetXY(0, 44);
    int16_t nameLen = glcdWriteString(buf);
    glcdDrawRect(nameLen, 44, rect.w - nameLen + 1, font->chars[0].image->height, pal->bg);
}
