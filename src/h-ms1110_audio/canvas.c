#include "gui/canvas.h"

#include "i2c.h"

#include "amp.h"
#include "audio/audio.h"
#include "display/glcd.h"
#include "input.h"
#include "menu.h"
#include "spectrum.h"
#include "swtimers.h"
#include "tr/labels.h"

#include <stdlib.h>
#include <string.h>

#define SPECTRUM_SIZE   43

static void drawMenuItem(uint8_t idx, const tFont *fontItem);

void canvasShowTune(bool clear)
{
    const Layout *lt = canvasGet()->layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
    }

    const int16_t value = aProc->par.tune[aProc->tune].value;

    const AudioGrid *grid = aProc->par.tune[aProc->tune].grid;
    const int8_t min = grid ? grid->min : 0;
    const int8_t max = grid ? grid->max : 0;
    const int8_t mStep = grid ? grid->mStep : 0;

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvasGet()->pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }

    // Bar
    StripedBar bar = {value, min, max};
    stripedBarDraw(&bar, &lt->tune.bar, clear);

    // Value
    glcdSetXY(lt->rect.w, lt->tune.valY);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdSetFont(lt->tune.valFont);
    glcdWriteString(utilMkStr("%3d", value * mStep / 8));
}

void canvasShowAudioInput(bool clear)
{
    const Layout *lt = canvasGet()->layout;

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    if (clear) {
        // Label
        glcdSetFont(lt->lblFont);
        glcdSetFontColor(canvasGet()->pal->fg);
        glcdSetXY(0, 0);
        glcdWriteString(label);
    }
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
    const Layout *lt = canvasGet()->layout;

    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = lt->rect.w;
    MenuIdx menuIdx = menu->list[idx];
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = lt->rect.h - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? canvasGet()->pal->fg : canvasGet()->pal->bg);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(canvasGet()->pal->fg);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }

    glcdWriteString(menuGetName(menuIdx));

    // Draw menu value
    int16_t x = canvasGet()->glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    // Inverse value color if selected
    color_t color = canvasGet()->glcd->fontFg;
    color_t bgColor = canvasGet()->glcd->fontBg;
    if (active && menu->selected) {
        glcdSetFontColor(bgColor);
        glcdSetFontBgColor(color);
    }
    glcdSetStringFramed(true);
    uint16_t strLen = glcdWriteString(menuGetValueStr(menuIdx));
    glcdSetStringFramed(false);

    glcdSetFontColor(color);
    glcdSetFontBgColor(bgColor);

    // Fill space between name and value
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, canvasGet()->pal->bg);
}

void canvasShowMenu(bool clear)
{
    (void)clear;

    const Layout *lt = canvasGet()->layout;

    Menu *menu = menuGet();

    const int16_t fHh = (int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = (int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (lt->rect.h - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(lt->menu.headFont);
    glcdSetFontColor(canvasGet()->pal->fg);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvasGet()->glcd->x, canvasGet()->glcd->y, lt->rect.w - canvasGet()->glcd->x, fHh, canvasGet()->pal->bg);

    glcdDrawRect(0, dividerPos, lt->rect.w, 1, canvasGet()->glcd->fontFg);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, lt->menu.menuFont);
        }
    }
}
