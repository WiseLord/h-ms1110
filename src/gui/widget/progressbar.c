#include "progressbar.h"

#include "gui/palette.h"

void progressBarDraw(ProgressBar *this, bool clear)
{
    (void)clear;

    const Palette *pal = paletteGet();

    const LayoutProgressBar *lt = &this->lt;
    const GlcdRect *rect = &lt->rect;
    const int16_t fw = lt->frame_width;

    const int16_t sc = lt->mark_count;
    const uint8_t sw = lt->mark_width;
    const int16_t barX = rect->x + fw + 1;
    const int16_t barY = rect->y + fw + 1;
    const int16_t barH = rect->h - 2 * (fw + 1);
    const int16_t width = rect->w - 1 - 2 * fw;

    int32_t value = this->value;
    int32_t min = this->min;
    int32_t max = this->max;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        color_t color = pal->fg;

        if (min == max) { // Empty scale
            color = this->bgColor;
        } else if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = this->bgColor;
            }
        } else { // Symmetric scale
            if ((value > 0 && i > value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = this->bgColor;
            }
        }

        glcdDrawRect(barX + i * (width / sc), barY, sw, barH, color);
    }

    glcdDrawRFrame(rect->x, rect->y, rect->w, rect->h, fw, fw + 1, COLOR_WHITE);
}
