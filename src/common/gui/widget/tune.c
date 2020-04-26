#include "tune.h"

#include "display/glcd.h"
#include "gui/palette.h"
#include "utils.h"

void tuneDraw(bool clear, Tune *tune, LayoutTune *lt)
{
    const Palette *pal = paletteGet();

    glcdSetFont(lt->valFont);
    glcdSetFontColor(pal->inactive);
    glcdSetXY(lt->valX, lt->valY);

    glcdWriteString(utilMkStr("%d", tune->value));
}
