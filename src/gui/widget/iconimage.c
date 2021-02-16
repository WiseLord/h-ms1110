#include "iconimage.h"

#include "gui/palette.h"

void iconImageDraw(IconImage *this, bool clear)
{
    if (!clear) {
        return;
    }

    const Icon icon = this->icon;
    const GlcdRect *rect = this->rect;
    const color_t color = this->color;

    const Palette *pal = paletteGet();
    const tImage *img = iconFind(icon, &icons_hms1110);

    glcdSetRect(rect);

    glcdSetXY(0, 0);
    glcdDrawImage(img, color, pal->bg);

    glcdResetRect();
}
