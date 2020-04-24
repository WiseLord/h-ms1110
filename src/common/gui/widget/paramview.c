#include "paramview.h"

#include "display/glcd.h"
#include "utils.h"

void paramViewDraw(bool clear, ParamView *pview, LayoutParamView *lt)
{
    glcdSetFont(lt->valFont);
    glcdSetXY(lt->valX, lt->valY);

    glcdWriteString(utilMkStr("%d", pview->val));
}
