#include "iconimage.h"

#include "gui/palette.h"

void iconImageDraw(bool clear, Icon icon, const GlcdRect *rect, color_t color)
{
    if (!clear) {
        return;
    }

    const Palette *pal = paletteGet();
    const tImage *img = iconFind(icon, &icons_hms1110);

    glcdSetRect(rect);

    glcdSetXY(0, 0);
    glcdDrawImage(img, color, pal->bg);

    glcdResetRect();
}
