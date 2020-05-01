#include "setupview.h"

#include "display/glcd.h"
#include "gui/palette.h"

void setupViewDraw(bool clear)
{
    const Palette *pal = paletteGet();

    GlcdRect rect = glcdGet()->rect;

    glcdSetFont(&fontterminus32);
    glcdSetFontColor(pal->fg);

    glcdSetXY(0, 0);
    glcdWriteString("setup");

    glcdSetXY(0, 32);
    glcdSetFontAlign(GLCD_ALIGN_LEFT);
    glcdWriteString("<<");

    glcdSetXY(rect.w, 32);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdWriteString(">>");
}
