#include "canvas.h"

#include "../audio/audio.h"
#include "../display/glcd.h"
#include "../input.h"
#include "../menu.h"
#include "../tr/labels.h"

static Canvas canvas;

void canvasInit()
{
    glcdInit(&canvas.glcd);

//    canvas.layout = layoutGet();

//    glcdDrawRect(0, 0, dispdrv.width, dispdrv.height, canvas.pal->bg);
    canvasClear();

//    canvas.glcd->rect = canvas.layout->rect;

    menuGet()->dispSize = 3/*canvas.layout->menu.itemCnt*/;
}

void canvasClear(void)
{
    GlcdRect rect = canvas.glcd->rect;

    glcdDrawRect(0, 0, rect.w, rect.h, LCD_COLOR_BLACK);
    glcdShift(0);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
}

void canvasShowSpectrum(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);

    glcdWriteString("Spectrum");
}

void canvasShowTime(bool clear)
{
    (void)clear;

    glcdSetFont(&fontterminus32);

    glcdSetXY(0, 0);

    glcdWriteString("Time");
}

void canvasShowStandby(bool clear)
{
    (void)clear;
    const AudioGrid *grid = audioGet()->par.tune[AUDIO_TUNE_BASS].grid;

    glcdSetFont(&fontterminus12);

    InputCtx *inCtx = inputGetCtx();

    const int16_t sw = 210;
    const int16_t zoneCnt = grid->max - grid->min + 1;

    for (uint8_t i = AIN_POT_A; i < AIN_POT_END; i++) {
        int8_t pot = inputGetPot(i);
        int16_t len = (sw * (pot - grid->min) + sw / 2) / zoneCnt;

        int16_t xb = 0;
        int16_t yb = 16 * i;

        glcdDrawRect(xb, yb, len, 12, LCD_COLOR_WHITE);
        glcdDrawRect(xb + len, yb, sw - len, 12, LCD_COLOR_GRAY);
        glcdSetXY(xb + sw + 2, yb);
        glcdWriteString(utilMkStr("%2d.%4d", pot, inCtx->potData[i]));
        for (int16_t j = 1; j < zoneCnt; j++) {
            glcdDrawRect(xb + j * sw / zoneCnt, yb, 1, 12, RGB_CONV(0xA0A0A0));
        }
    }

    AnalogBtn aBtn = inputGetCtx()->aBtn;

    for (AnalogBtn i = 0; i < ABTN_END; i++) {
        glcdDrawCircle(16 + 20 * i, 40, 6, i == aBtn ? LCD_COLOR_WHITE : LCD_COLOR_GRAY);
    }
}

void canvasShowTune(bool clear)
{
    (void)clear;
    canvasClear();

    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    const int16_t value = aProc->par.tune[aProc->tune].value;
//    Icon icon = (ICON_TUNER + inType);

    if (aProc->tune < AUDIO_TUNE_GAIN) {
        label = labelsGet(LABEL_VOLUME + aProc->tune);
//        icon = ICON_VOLUME + aProc->tune;
    }

    const AudioGrid *grid = aProc->par.tune[aProc->tune].grid;
//    const int8_t min = grid ? grid->min : 0;
//    const int8_t max = grid ? grid->max : 0;
    const uint8_t mStep = grid ? grid->mStep : 0;


    glcdSetFont(&fontterminus32);
    glcdSetXY(0, 0);
    glcdWriteString(label);

    glcdSetFont(&fontterminus32);
    glcdSetXY(256, 0);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);
    glcdWriteString(utilMkStr("%3d", value * mStep / 8));
}

void canvasShowAudioInput(bool clear)
{
    AudioProc *aProc = audioGet();
    InputType inType = aProc->par.inType[aProc->par.input];

    const char *label = labelsGet(LABEL_IN_TUNER + inType);

    glcdSetFont(&fontterminus32);
    glcdSetXY(0, 0);
    glcdWriteString(label);
}

static void drawMenuItem(uint8_t idx, const tFont *fontItem)
{
//    const Layout *lt = canvas.layout;

    uint8_t fIh = (uint8_t)fontItem->chars[0].image->height;

    Menu *menu = menuGet();
    uint8_t items = menu->dispSize;

    int16_t width = 256/*lt->rect.w*/;
    MenuIdx menuIdx = menu->list[idx];
    uint8_t active = (menu->active == menu->list[idx]);

    const uint8_t ih = fIh + 4; // Menu item height
    int16_t y_pos = 64/*lt->rect.h*/ - ih * (items - idx + menu->dispOft);

    // Draw selection frame
    glcdDrawFrame(0, y_pos, width, ih, 1, active ? LCD_COLOR_WHITE/*canvas.pal->fg*/ : LCD_COLOR_BLACK);

    // Draw menu name
    glcdSetFont(fontItem);
    glcdSetFontColor(LCD_COLOR_WHITE/*canvas.pal->fg*/);

    glcdSetXY(4, y_pos + 2);
    if (menu->list[idx] != MENU_NULL) {
        glcdWriteString("  ");
    } else {
        glcdWriteString("< ");
    }

    glcdWriteString(menuGetName(menuIdx));

    // Draw menu value
    int16_t x = canvas.glcd->x;
    glcdSetXY(width - 2, y_pos + 2);
    glcdSetFontAlign(GLCD_ALIGN_RIGHT);

    // Inverse value color if selected
    color_t color = canvas.glcd->fontFg;
    color_t bgColor = canvas.glcd->fontBg;
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
    glcdDrawRect(x, y_pos + 2, width - 2 - x - strLen, fIh, LCD_COLOR_BLACK/*canvas.pal->bg*/);
}

void canvasShowMenu(bool clear)
{
    (void)clear;

//    const Layout *lt = canvas.layout;

    Menu *menu = menuGet();

    const int16_t fHh = 16;//(int16_t)lt->menu.headFont->chars[0].image->height;
    const int16_t fIh = 12;//(int16_t)lt->menu.menuFont->chars[0].image->height;
    const uint8_t items = menu->dispSize;

    const int16_t dividerPos = (64/*lt->rect.h*/ - (fIh + 4) * items + fHh) / 2;

    // Show header
    const char *parentName = menuGetName(menu->parent);
    glcdSetFont(&fontterminus16/*lt->menu.headFont*/);
    glcdSetFontColor(LCD_COLOR_WHITE/*canvas.pal->fg*/);

    glcdSetXY(2, 0);
    glcdWriteString(parentName);
    // Fill free space after header
    glcdDrawRect(canvas.glcd->x, canvas.glcd->y, 256/*lt->rect.w*/ - canvas.glcd->x, fHh, LCD_COLOR_BLACK/*canvas.pal->bg*/);

    glcdDrawRect(0, dividerPos, 256/*lt->rect.w*/, 1, canvas.glcd->fontFg);

    for (uint8_t idx = 0; idx < menu->listSize; idx++) {
        if (idx >= menu->dispOft && idx < items + menu->dispOft) {
            drawMenuItem(idx, &fontterminus12/*lt->menu.menuFont*/);
        }
    }
}
