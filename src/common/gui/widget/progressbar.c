#include "progressbar.h"

#include "gui/palette.h"

void progressBarDraw(bool clear, ProgressBar *bar)
{
    (void)clear;

    const Palette *pal = paletteGet();

    const LayoutProgressBar *lt = &bar->lt;
    const GlcdRect *rect = &lt->rect;
    const int16_t fw = lt->fw;

    const int16_t sc = lt->sc;         // Scale count
    const uint8_t sw = lt->sw;         // Scale width
    const int16_t barPos = rect->y + fw + 1;
    const int16_t barH = rect->h - 2 * (fw + 1);
    const int16_t width = rect->w - 1 - 2 * fw;

    int16_t value = bar->value;
    int16_t min = bar->min;
    int16_t max = bar->max;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        color_t color = pal->fg;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = pal->inactive;
            }
        } else { // Symmetric scale
            if ((value > 0 && i > value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = pal->inactive;
            }
        }

        glcdDrawRect(4 + i * (width / sc), barPos, sw, barH, color);
    }

    glcdDrawRFrame(rect->x, rect->y, rect->w, rect->h, fw, 3, COLOR_WHITE);
}