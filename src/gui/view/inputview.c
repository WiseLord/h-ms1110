#include "inputview.h"

#include <stdio.h>

#include "amp.h"
#include "display/glcd.h"
#include "gui/palette.h"
#include "gui/widget/iconimage.h"

static const GlcdRect rectName = {56, 4, 200, 32};
static const int16_t xBase = 108;

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

static void drawIcon(InputView *this, int16_t xPos, int8_t inIdx)
{
    const InputMap *inMap = this->inMap;
    const Palette *pal = paletteGet();

    int16_t delta = (xBase - xPos) / 6;
    if (delta < 0) {
        delta = -delta;
    }
    if (delta > 14) {
        delta = 14;
    }
    delta = 15 - delta;

    color_t color = COLOR_GRAY16(delta);


    Icon icon = ICON_TUNER + (inMap->pairs[inIdx].type - IN_TUNER);

    const tImage *img = iconFind(icon, &icons_hms1110);

    glcdSetXY(xPos, 0);
    glcdDrawImage(img, color, pal->bg);
}

void inputViewDraw(InputView *this, bool clear)
{
    int16_t shiftX = 0;
    const InputMap *inMap = this->inMap;
    const Palette *pal = paletteGet();

    if (this->scrollTimer >= 0) {
        shiftX = this->scrollTimer / 2; // Max 60
        clear = true;
    }

    if (!clear) {
        return;
    }

    if (this->inIdxUp) {
        shiftX = -shiftX;
    }

    for (int8_t i = -3; i <= 3; i++) {
        drawIcon(this, xBase + shiftX - (60 * i), (inMap->mapSize + this->inIdx + i) % inMap->mapSize);
    }

    for (int8_t i = -2; i <= 3; i++) {
        glcdDrawRect(xBase + shiftX + (60 * i) - 20, 0, 20, 40, pal->bg);

    }
}
