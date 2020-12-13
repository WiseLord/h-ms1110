#include "radioview.h"

#include <stdio.h>

#include "display/glcd.h"
#include "gui/palette.h"

#include "tuner/rds/parser.h"

void radioViewDraw(bool clear, RadioView *radio)
{
    (void)clear;

    const Palette *pal = paletteGet();
    const tFont *font = &fontterminus32;

    glcdSetFont(font);
    glcdSetFontColor(pal->active);

    uint16_t freq = radio->freq;

    char buf[80];

    snprintf(buf, sizeof(buf), "FM %3u.%02u", freq / 100, freq % 100);
    glcdSetXY(0, 0);
    glcdWriteString(buf);

    font = &fontterminus14b;
    glcdSetFont(font);

    RdsParser *parser = rdsParserGet();

    snprintf(buf, sizeof(buf), "%s: %s", parser->PS, parser->text);
    glcdSetXY(0, 30);
    glcdWriteString(buf);

    snprintf(buf, sizeof(buf), "Stereo: %d", radio->stereo);
    glcdSetXY(0, 44);
    glcdWriteString(buf);

    if (radio->stationNum >= 0) {
        snprintf(buf, sizeof(buf), "Station: %02d", radio->stationNum + 1);
    } else {
        snprintf(buf, sizeof(buf), "Station: --");
    }
    glcdSetXY(100, 44);
    glcdWriteString(buf);

    if (radio->station) {
        glcdWriteString(" +");
    } else {
        glcdWriteString(" -");
    }

    for (int8_t i = 0; i < 3; i++) {
        for (int8_t j = 0; j < 3; j++) {
            if (1 << (1 + i * 3 + j) & radio->favMask) {
                glcdDrawRing(206 + 20 * j, 12 + 20 * i, 8, 2, pal->active);
            } else {
                glcdDrawRing(206 + 20 * j, 12 + 20 * i, 8, 2, pal->inactive);
            }
        }
    }
}
