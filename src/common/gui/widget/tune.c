#include "tune.h"

#include "display/glcd.h"
#include "utils.h"

void tuneDraw(bool clear, Tune *tune, LayoutTune *lt)
{
    glcdSetFont(lt->valFont);
    glcdSetXY(lt->valX, lt->valY);

    glcdWriteString(utilMkStr("%d", tune->value));
}
