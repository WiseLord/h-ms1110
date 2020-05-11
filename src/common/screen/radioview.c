#include "radioview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"

void radioViewDraw(bool clear, RadioView *radio)
{
    (void)clear;

    const Palette *pal = paletteGet();
    const tFont *font = &fontterminus32;

    glcdSetFont(font);
    glcdSetFontColor(pal->active);

    uint16_t freq = radio->freq;

    char buf[20];

    snprintf(buf, sizeof(buf), "FM %3u.%02u", freq / 100, freq % 100);
    glcdSetXY(0, 0);
    glcdWriteString(buf);

    font = &fontterminus20;
    glcdSetFont(font);

    snprintf(buf, sizeof(buf), "Stereo: %d", radio->stereo);
    glcdSetXY(0, 44);
    glcdWriteString(buf);
}
